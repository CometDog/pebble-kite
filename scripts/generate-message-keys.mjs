#!/usr/bin/env node

import { readFileSync, writeFileSync } from "fs";
import { createRequire } from "module";
import { fileURLToPath } from "url";
import path from "path";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(__dirname, "..");

const require = createRequire(import.meta.url);
const keyNames = require("./message-keys.js");

// Detect duplicate key names
const seen = new Set();
for (const name of keyNames) {
  if (seen.has(name)) {
    console.error(
      `[generate-message-keys] ERROR: Duplicate key "${name}" in message-keys.js`,
    );
    process.exit(1);
  }
  seen.add(name);
}

const pkgPath = path.join(root, "package.json");
const pkg = JSON.parse(readFileSync(pkgPath, "utf8"));

const KEY_FLOOR = 10501;

// Map of existing keys in package.json over the KEY_FLOOR value
const existing = new Map();
for (const [name, id] of Object.entries(pkg.pebble.messageKeys)) {
  if (id >= KEY_FLOOR) {
    existing.set(name, id);
  }
}

// Warn about keys in package.json that are not in message-keys.js; remove these keys
for (const name of existing.keys()) {
  if (!keyNames.includes(name)) {
    console.warn(
      `[generate-message-keys] WARNING: "${name}" (id ${existing.get(name)}) ` +
        `is in package.json but not in scripts/message-keys.js. It will be removed.`,
    );
  }
}

// Assign IDs to new keys starting from KEY_FLOOR, skipping already used values
const usedIds = new Set(existing.values());
let nextId = KEY_FLOOR;
const getNextId = () => {
  while (usedIds.has(nextId)) nextId++;
  usedIds.add(nextId);
  return nextId++;
};

const assigned = new Map();
for (const name of keyNames) {
  if (existing.has(name)) {
    assigned.set(name, existing.get(name));
  } else {
    const id = getNextId();
    console.log(`[generate-message-keys] Assigned new key "${name}" to ${id}`);
    assigned.set(name, id);
  }
}

// Always put these keys at the end of the list in package.json
const clayKeys = Object.fromEntries(
  Object.entries(pkg.pebble.messageKeys).filter(([, id]) => id < KEY_FLOOR),
);
const protocolKeys = Object.fromEntries(assigned);

pkg.pebble.messageKeys = { ...clayKeys, ...protocolKeys };

writeFileSync(pkgPath, JSON.stringify(pkg, null, 2) + "\n");
console.log("[generate-message-keys] package.json updated.");
