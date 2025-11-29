export const replaceAll = (
  str: string,
  find: string | RegExp,
  replace: string,
) => {
  return str.replace(new RegExp(find, "g"), replace);
};
