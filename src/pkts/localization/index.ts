import { strings as enStrings } from "./definitions/base";
import { strings as frStrings } from "./definitions/fr";
import { strings as deStrings } from "./definitions/de";
import { strings as esStrings } from "./definitions/es";
import { strings as itStrings } from "./definitions/it";
import { strings as ptStrings } from "./definitions/pt";
import { InterfaceStrings } from "./type";

type DeepPartial<B> = {
  [X in keyof B]?: B[X] extends object ? DeepPartial<B[X]> : B[X];
};

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

export const filterInterfaceStringSectionsAndCategories = ({
  strings,
  sectionKeys,
  categoryKeys,
}: {
  strings: InterfaceStrings;
  sectionKeys: string[];
  categoryKeys: string[];
}): DeepPartial<InterfaceStrings> => {
  const filteredStrings: DeepPartial<InterfaceStrings> = {
    title: { ...strings.title },
    section: { sources: strings.section.sources },
    category: {},
  };

  console.log(
    "Filtering interface strings with sections:",
    sectionKeys,
    "and categories:",
    categoryKeys,
  );
  // Filter sections to only include those in sectionKeys
  for (const [key, value] of Object.entries(enStrings.section)) {
    if (sectionKeys.includes(value)) {
      // Force unwrap, we know it exists from initialization above
      filteredStrings!.section![key as keyof typeof strings.section] =
        strings.section[key as keyof typeof strings.section];
    }
  }

  // Filter categories to only include those in categoryKeys
  for (const [key, value] of Object.entries(enStrings.category)) {
    if (categoryKeys.includes(value)) {
      // Force unwrap, we know it exists from initialization above
      filteredStrings!.category![key as keyof typeof strings.category] =
        strings.category[key as keyof typeof strings.category];
    }
  }

  return filteredStrings;
};

export const getFlattenedInterfaceStrings = (
  lang?: string,
): Record<string, string> => {
  const langToUse = lang || navigator.language || "en";
  const interfaceStrings = getInterfaceStrings(langToUse);
  return flattenInterfaceStrings(interfaceStrings);
};

export function flattenInterfaceStrings(
  strings: InterfaceStrings | DeepPartial<InterfaceStrings>,
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
