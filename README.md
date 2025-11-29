Project is powered by and based on Kagi News:
https://help.kagi.com/kagi/news/

TypeScript working is based on work here:
https://github.com/jccit/PebbleKit.ts
https://github.com/C-D-Lewis/pebble-dev/commit/6fc00db08b10b87c9656d4386f94ea004cdfdceb

This entire project is going to change drastically every time I push code to it until I settle on a true design for the code base. Until then I'm force pushing straight to main and no one can stop me.

## What's Going On In This Code?

I write new code in chaos where I throw the first thing I can think of at the wall as fast as possible until it works then iterate on that over and over until it looks the way I want it to while still working. That has landed me with a few oddities:

1. I've included a polyfill for `Promise` even though it is "supposed" to work. However without it, I am seeing that the network requests through the `fetch`-like wrappers that go to the XMLHttpRequests end up never resolving. With it, they resolve. It works and I don't plan to look at it any time soon
1. I have a seemingly unnecessary dispatch queue for events over the Phone>Watch bridge. This was originally built because early in development I was erroneously triggering a few events in parallel right as the app started which was causing problems so I added the dispatch queue to allow requests to wait instead of just being dropped. This isn't actually necessary to keep anymore but it works. I don't plan to get rid of it because it doesn't hurt and it does do something useful.
1. I have a lot of different ways to abstract similar things. Originally I was just copy-pasting the same screens over and over which worked but was annoying to support. From there I moved the data management to a system that would be able to request the appropriate data for the specific screen that was about to come into view. After I consolidated the data requests on the phone side, this became more and more obsolete. Then I added a navigation management system which almost completely made the data maangement system redundant. I will clean this up and it's a constant point of conflict with myself.
1. I have like 3 different patterns across the code base for file structure. I just haven't picked one yet

## Goals

The project has a few goals with its design

1. An extremely generic UI view that is able to load data in and out based on the requested position in the app
1. As small of a memory + dispatch bridge footprint as possible
1. Support back to Aplite.
   a. This doesn't exist but the dream drives a lot of my optimizations even if it won't ever happen.
1. Quick support of new API responses from the Kagi API
