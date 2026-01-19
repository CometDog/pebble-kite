import { cacheFirstFetchText } from "../cache/cacheFirstFetch";
import { Story } from "../type/Story";

export type SmallWebEntry = Pick<
  Story,
  "title" | "id" | "short_summary" | "articles"
>;

export type SmallWebFeedResponse = {
  entries: SmallWebEntry[];
};

const decodeHtmlEntities = (text: string): string => {
  return (
    text
      // Decode HTML entities
      .replace(/&lt;/g, "<")
      .replace(/&gt;/g, ">")
      .replace(/&amp;/g, "&")
      .replace(/&quot;|&#34;/g, '"')
      .replace(/&apos;|&#39;/g, "'")
      .replace(/&#(\d+);/g, (_match, dec) =>
        String.fromCharCode(parseInt(dec, 10)),
      )
      .replace(/&#x([0-9a-fA-F]+);/g, (_match, hex) =>
        String.fromCharCode(parseInt(hex, 16)),
      )
      // Convert line breaks and paragraphs to newlines
      .replace(/<br\s*\/?>/gi, "\n")
      .replace(/<\/p>/gi, "\n") // Just use a single line break for paragraph breaks for visual simplicity
      .replace(/<p[^>]*>/gi, "")
      // Strip all remaining HTML tags
      .replace(/<[^>]+>/g, "")
      // Clean up excessive whitespace
      .replace(/\n{3,}/g, "\n\n")
      .trim()
  );
};

const parseAtomFeed = (xmlText: string): SmallWebFeedResponse => {
  const entries: SmallWebEntry[] = [];

  // Match all <entry>...</entry> blocks
  const entryRegex = /<entry[^>]*>([\s\S]*?)<\/entry>/g;
  let match;

  while ((match = entryRegex.exec(xmlText)) !== null) {
    const entryXml = match[1];

    const getTagContent = (tag: string): string => {
      const tagRegex = new RegExp(`<${tag}[^>]*>([\\s\\S]*?)<\\/${tag}>`, "i");
      const tagMatch = entryXml.match(tagRegex);
      return tagMatch ? tagMatch[1].trim() : "";
    };

    const getHrefAttr = (): string => {
      const linkRegex = /<link[^>]+href=["']([^"']+)["']/i;
      const linkMatch = entryXml.match(linkRegex);
      return linkMatch ? linkMatch[1] : "";
    };

    entries.push({
      title: decodeHtmlEntities(getTagContent("title")),
      id: getTagContent("id"),
      short_summary: decodeHtmlEntities(getTagContent("summary")),
      articles: [
        {
          link: getHrefAttr(),
          domain: getHrefAttr()
            .replace(/^https?:\/\/(www\.)?/, "")
            .split("/")[0],
          image: "",
          image_caption: "",
          title: decodeHtmlEntities(getTagContent("title")),
          date: getTagContent("published"),
        },
      ],
    });
  }

  return { entries };
};

export const smallWebFeedRequest = async (
  limit = 12,
): Promise<SmallWebFeedResponse> => {
  const url = `https://kagi.com/api/v1/smallweb/feed?limit=${limit + 6}`; // Request a few extra to account for filtering

  const xmlText = await cacheFirstFetchText(url);
  return {
    entries: parseAtomFeed(xmlText)
      .entries.filter((entry) => entry.short_summary.length > 0)
      .slice(0, limit),
  };
};
