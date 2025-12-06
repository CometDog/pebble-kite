import { strings as enStrings } from "./definitions/base";
import { strings as frStrings } from "./definitions/fr";
import { strings as deStrings } from "./definitions/de";
import { strings as esStrings } from "./definitions/es";
import { strings as itStrings } from "./definitions/it";
import { strings as ptStrings } from "./definitions/pt";
import { InterfaceStrings } from "./type";

export const getInterfaceStrings = (lang?: string): InterfaceStrings => {
  const langToUse = lang || navigator.language || "en";
  const langCode = langToUse.split("_")[0].split("-")[0];
  switch (langCode) {
    case "en":
      return enStrings;
    case "de":
      return deStrings;
    case "es":
      return esStrings;
    case "fr":
      return frStrings;
    case "it":
      return itStrings;
    case "pt":
      return ptStrings;
    default:
      return enStrings;
  }
};

export const getFlattenedInterfaceStrings = (
  lang?: string,
): Record<string, string> => {
  const langToUse = lang || navigator.language || "en";
  const interfaceStrings = getInterfaceStrings(langToUse);
  return flattenInterfaceStrings(interfaceStrings);
};

export function flattenInterfaceStrings(
  strings: InterfaceStrings,
): Record<string, string> {
  const flatStrings: Record<string, string> = {};

  if (!strings) return flatStrings;

  if (strings.title) {
    if (strings.title.category !== undefined) {
      flatStrings["title.category"] = strings.title.category;
    }
    if (strings.title.stories !== undefined) {
      flatStrings["title.stories"] = strings.title.stories;
    }
    if (strings.title.availableDetails !== undefined) {
      flatStrings["title.availableDetails"] = strings.title.availableDetails;
    }
  }

  if (strings.section) {
    for (const [key, value] of Object.entries(strings.section)) {
      if (typeof value === "string") {
        const newKey = `section.${key}`;
        flatStrings[newKey] = value;
      }
    }
  }

  if (strings.category) {
    for (const [key, value] of Object.entries(strings.category)) {
      if (typeof value === "string") {
        const newKey = `category.${key}`;
        flatStrings[newKey] = value;
      }
    }
  }

  return flatStrings;
}
