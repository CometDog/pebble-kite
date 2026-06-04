import { createLogger } from "../lib";
import { onThisDayRequest } from "./server/request/contentRequests";

const log = createLogger("KNJSTimeline");

const insertTimelinePin = (pin: TimelinePin) => {
  // We cannot rely on interception from the Core app because it will break the fetch call since
  // it expects an XHR request. Try using the native function if it exists first, only falling back
  // so that other apps supporting the timeline API still function
  if (Pebble.insertTimelinePin && PebbleTS.insertTimelinePin) {
    PebbleTS.insertTimelinePin(pin);
  } else {
    // As I do not have access to an app that supports the timeline API, this is untested by me
    PebbleTS.getTimelineToken().then((timelineToken) => {
      if (!timelineToken) {
        log.error("No timeline token found");
        return;
      }

      fetch("https://timeline-api.rebble.io/v1/user/pins/" + pin.id, {
        method: "PUT",
        headers: {
          "Content-Type": "application/json",
          "X-User-Token": timelineToken,
        },
        body: JSON.stringify(pin),
      });
    });
  }
};

const extractTextFromHtml = (html: string): string => {
  if (!html) return "";

  let text = html.replace(/<\/?[^>]+(>|$)/g, "");
  text = text.replace(/&#(\d+);/g, (_, n) =>
    String.fromCharCode(parseInt(n, 10)),
  );
  text = text.replace(/&#x([\da-fA-F]+);/g, (_, n) =>
    String.fromCharCode(parseInt(n, 16)),
  );
  text = text
    .replace(/&nbsp;/g, " ")
    .replace(/&amp;/g, "&")
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&quot;/g, '"')
    .replace(/&apos;/g, "'");
  return text.replace(/\s+/g, " ").trim();
};

export const pushNewsRefreshPinToTimeline = () => {
  // Retrieve the On This Day for the latest batch always to avoid waiting around for the batch to update here
  onThisDayRequest({ batchId: "latest" }).then((onThisDay) => {
    const currentTime = new Date();
    // Update time is UTC 12 PM
    let utcUpdateTime1 = Date.UTC(
      currentTime.getUTCFullYear(),
      currentTime.getUTCMonth(),
      currentTime.getUTCDate(),
      12, // 12 PM UTC, the update might be late, but this is when it is scheduled and I want to give the user time to react to the quick view event and read today's news before it is gone
      0,
      0,
    );

    const oneDayMs = 24 * 60 * 60 * 1000;
    // Just trying to not always grab the same position in the events array
    const events = onThisDay.events.sort(() => Math.random() - 0.5).slice(0, 3);

    const genericPin = (
      id: string,
      timeMs: number,
      body: string,
    ): TimelinePin => ({
      id: `kagi-news-refresh-time-pin-${id}`,
      time: new Date(timeMs),
      layout: {
        type: "genericPin",
        title: "Kagi News Refresh",
        subtitle: "Week In History", // Week In History instead of On This Day because I want to pin future days with current batches
        body: body.slice(0, 512),
        tinyIcon: "system://images/NEWS_EVENT", // Unable to use custom icons for now due to firmware bug
      },
      actions: [], // Actions do not work, ideally would have a "View More" action to open the app
    });

    const times = [
      utcUpdateTime1,
      utcUpdateTime1 + oneDayMs,
      utcUpdateTime1 + oneDayMs * 2,
    ];

    // If a user removed a pin already I don't want to re-add it so I rely on checking if I've already pushed this time
    const lastPushTimes = times.map((_, index) =>
      localStorage.getItem(`newsRefreshPin${index}PushTime`),
    );
    const needNewPush = times.map((timeMs) => {
      if (lastPushTimes.includes(timeMs.toString())) return false;
      return true;
    });

    const insertPinForIndex = (index: number) => {
      if (index === 0 && currentTime.getTime() >= utcUpdateTime1) return; // Skip if the time has already passed for today
      const event = events.pop();
      const timelineBody = event?.content
        ? `${event.year}: ${extractTextFromHtml(event.content)}`
        : "Nothing happened!"; // This should probably be more meaningful but it is rare and I hope not upsetting to anyone as it is just additional info to make the pin more interesting
      const pin = genericPin(String(index + 1), times[index], timelineBody);
      insertTimelinePin(pin);
      localStorage.setItem(
        `newsRefreshPin${index}PushTime`,
        times[index].toString(),
      );
    };

    for (let i = 0; i < times.length; i++) {
      if (needNewPush[i]) {
        insertPinForIndex(i);
      }
    }
  });
};
