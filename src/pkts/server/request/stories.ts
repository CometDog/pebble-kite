import { fetchString } from "../../../pktslib/fetch";

export const storiesRequest = ({ categoryId }: { categoryId: string }) =>
  fetchString(
    `https://news.kagi.com/api/batches/latest/categories/${categoryId}/stories`,
  );
