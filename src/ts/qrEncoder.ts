import qrcode from "qrcode-generator";

export function generateQrCode(data: string): {
  matrix: boolean[][];
  size: number;
} {
  const qr = qrcode(0, "L");
  qr.addData(data);
  qr.make();

  const size = qr.getModuleCount();
  const matrix: boolean[][] = [];

  for (let y = 0; y < size; y++) {
    const row: boolean[] = [];
    for (let x = 0; x < size; x++) {
      row.push(qr.isDark(y, x));
    }
    matrix.push(row);
  }

  return { matrix, size };
}

export function qrMatrixToBytes(matrix: boolean[][]): number[] {
  const size = matrix.length;
  const byteSize = Math.ceil((size * size) / 8);
  const bytes = new Array(byteSize).fill(0);

  for (let y = 0; y < size; y++) {
    for (let x = 0; x < size; x++) {
      if (matrix[y][x]) {
        const bitIndex = y * size + x;
        const byteIndex = Math.floor(bitIndex / 8);
        const bitPosition = 7 - (bitIndex % 8);
        bytes[byteIndex] |= 1 << bitPosition;
      }
    }
  }

  return bytes;
}
