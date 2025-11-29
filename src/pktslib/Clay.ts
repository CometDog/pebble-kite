/**
 * Generic option used in selects/radios
 */
interface ClayOption {
  label?: string;
  value?: string | number | boolean;
  [propName: string]: any;
}

/**
 * A fairly permissive representation of a Clay item/control.
 * Specific control types may add more fields; keep this flexible.
 */
interface ClayItem {
  type?: string;
  messageKey?: string;
  label?: string;
  defaultValue?: any;
  description?: string;
  items?: ClayItem[];
  options?: ClayOption[];
  [propName: string]: any;
}

type ClayConfig = ClayItem[];

interface Clay {
  /**
   * Returns the URL to open in the webview for configuration.
   */
  generateUrl(): string;

  /**
   * Parse response from the webview and return a plain settings object.
   * Example: { UseCategoryFilter: true, SelectedCategories: 'World,Science' }
   */
  getSettings(response: string): { [key: string]: any };

  [propName: string]: any;
}

export interface ClayConstructor {
  new (
    config: ClayConfig | object,
    localize?: any,
    options?: { autoHandleEvents?: boolean },
  ): Clay;
}
