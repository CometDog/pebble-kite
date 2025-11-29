import typescript from "@rollup/plugin-typescript";
import nodeResolve from "@rollup/plugin-node-resolve";
import babel from "@rollup/plugin-babel";
import commonjs from "@rollup/plugin-commonjs";
import terser from "@rollup/plugin-terser";
import inject from "@rollup/plugin-inject";

export default [
  {
    input: "src/pktsconfig/config.ts",
    output: {
      file: "src/pkjs/config.js",
      format: "cjs",
      compact: true,
    },
    plugins: [typescript(), commonjs(), terser()],
  },
  {
    input: "src/pkts/index.ts",
    output: {
      file: "src/pkjs/index.js",
      format: "iife",
      compact: true,
    },
    plugins: [
      inject({
        PebbleTS: new URL("src/pktslib/index.ts", import.meta.url).pathname,
      }),
      typescript(),
      nodeResolve(),
      commonjs(),
      babel({ babelHelpers: "inline", extensions: [".ts", ".js"] }),
      terser(),
    ],
  },
];
