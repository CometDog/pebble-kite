import { generateQrCode, qrMatrixToBytes } from "./qrEncoder";
import { fetchString } from "../pktslib/fetch";
import { QR_CODE_CHUNK_SIZE } from "./types";

// Max Size a QR code can be for watch app to handle it properly
const MAX_QR_SIZE = 64;
// Max size for comfortable viewing on small watch screens (larger QR codes are fine to scan but I don't like the look of them)
const COMFORTABLE_QR_SIZE = 48;

let currentQrCodeData: {
  bytes: number[];
  totalChunks: number;
  size: number;
} | null = null;

export const generateQrBytes = (data: string) => {
  const { matrix, size } = generateQrCode(data);
  const bytes = qrMatrixToBytes(matrix);
  return { bytes, size };
};

const shortenUrl = async (url: string): Promise<string> => {
  const shortenerUrl = `https://is.gd/create.php?format=simple&url=${encodeURIComponent(url)}`;
  return await fetchString(shortenerUrl);
};

export const updateQrCodeBitmap = async ({ url }: { url: string }) => {
  currentQrCodeData = null;
  console.log(
    `[KNJSQR]: Generating QR for URL (${url.length} chars): ${url.substring(0, 50)}...`,
  );

  // First, try generating QR code directly
  let { bytes, size } = generateQrBytes(url);
  console.log(
    `[KNJSQR]: Initial QR size: ${size}x${size}, MAX_QR_SIZE=${MAX_QR_SIZE}`,
  );

  // If QR code is to be comfortable to scan on a watch, try shortening the URL
  if (size > COMFORTABLE_QR_SIZE) {
    console.log(
      `[KNJSQR]: QR code too large (${size}x${size}), shortening URL...`,
    );
    try {
      const shortUrl = await shortenUrl(url);
      console.log(`[KNJSQR]: Shortened URL: ${shortUrl}`);
      const shortened = generateQrBytes(shortUrl);
      bytes = shortened.bytes;
      size = shortened.size;
      console.log(`[KNJSQR]: After shortening: ${size}x${size}`);
    } catch (err) {
      console.error(
        "[KNJSQR]: URL shortening failed, using original URL:",
        err,
      );
      if (size > MAX_QR_SIZE) {
        // Give up if all else fails, it'll never work
        return { bytes: [], totalChunks: 0, size: 0 };
      } else {
        // Just use the original QR code data if it's within the bounds of the max size
        console.log(
          `[KNJSQR]: Original QR code size (${size}x${size}) is within max limit (${MAX_QR_SIZE})`,
        );
      }
    }
  }

  const totalChunks = Math.ceil(bytes.length / QR_CODE_CHUNK_SIZE);
  currentQrCodeData = { bytes, totalChunks, size };
  console.log(`[KNJSQR]: Final: size=${size}, totalChunks=${totalChunks}`);
  return currentQrCodeData;
};

export const getQrCodeChunk = (chunk: number) => {
  if (!currentQrCodeData) return null;
  const idx = chunk - 1;
  const chunkData = currentQrCodeData.bytes.slice(
    QR_CODE_CHUNK_SIZE * idx,
    QR_CODE_CHUNK_SIZE * (idx + 1),
  );
  const isNext = currentQrCodeData.totalChunks > chunk;
  return { chunkData, isNext, nextChunk: isNext ? chunk + 1 : undefined };
};
