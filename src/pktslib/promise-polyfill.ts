export type Executor<T> = (
  resolve: (value?: T | PromiseLike<T>) => void,
  reject: (reason?: any) => void,
) => void;
export type OnFulfilled<T, U> =
  | ((value: T) => U | PromiseLike<U>)
  | null
  | undefined;
export type OnRejected<U> =
  | ((reason: any) => U | PromiseLike<U>)
  | null
  | undefined;

enum State {
  Pending = 0,
  Fulfilled = 1,
  Rejected = 2,
}

const enqueueMicrotask = (function () {
  if (
    typeof MutationObserver !== "undefined" &&
    typeof document !== "undefined"
  ) {
    const queue: (() => void)[] = [];
    const node = (document as any).createTextNode("");
    const mo = new MutationObserver(() => {
      const toRun = queue.splice(0);
      for (const fn of toRun) fn();
    });
    mo.observe(node, { characterData: true });
    return (fn: () => void) => {
      queue.push(fn);
      node.data = String(Number(node.data) ^ 1);
    };
  }

  if (typeof MessageChannel !== "undefined") {
    const channel = new MessageChannel();
    const queue: (() => void)[] = [];
    channel.port1.onmessage = () => {
      const toRun = queue.splice(0);
      for (const fn of toRun) fn();
    };
    return (fn: () => void) => {
      queue.push(fn);
      channel.port2.postMessage(0);
    };
  }

  return (fn: () => void) => setTimeout(fn, 0);
})();

function isThenable(x: any): x is PromiseLike<any> {
  return (
    x &&
    (typeof x === "object" || typeof x === "function") &&
    typeof x.then === "function"
  );
}

export class PromisePolyfill<T> implements PromiseLike<T> {
  private _state: State = State.Pending;
  private _value: any = undefined;
  private _handlers: Array<{
    onFulfilled?: Function;
    onRejected?: Function;
    resolve: Function;
    reject: Function;
  }> = [];

  constructor(executor: Executor<T>) {
    if (typeof executor !== "function") {
      throw new TypeError("Promise executor is not a function");
    }

    const resolve = (value?: T | PromiseLike<T>) => this._resolve(value);
    const reject = (reason?: any) => this._reject(reason);

    try {
      executor(resolve, reject);
    } catch (err) {
      reject(err);
    }
  }

  private _resolve(value?: any) {
    if (this._state !== State.Pending) return;

    if (value === this) {
      return this._reject(new TypeError("Cannot resolve promise with itself"));
    }

    try {
      if (isThenable(value)) {
        // adopt state from thenable
        value.then(
          (v: any) => this._resolve(v),
          (r: any) => this._reject(r),
        );
        return;
      }
    } catch (err) {
      this._reject(err);
      return;
    }

    this._state = State.Fulfilled;
    this._value = value;
    this._finalize();
  }

  private _reject(reason?: any) {
    if (this._state !== State.Pending) return;
    this._state = State.Rejected;
    this._value = reason;
    this._finalize();
  }

  private _finalize() {
    enqueueMicrotask(() => {
      for (const h of this._handlers) {
        this._callHandler(h);
      }
      this._handlers = [];
    });
  }

  private _callHandler(h: {
    onFulfilled?: Function;
    onRejected?: Function;
    resolve: Function;
    reject: Function;
  }) {
    try {
      if (this._state === State.Fulfilled) {
        if (!h.onFulfilled) {
          h.resolve(this._value);
        } else {
          const result = h.onFulfilled(this._value);
          h.resolve(result);
        }
      } else if (this._state === State.Rejected) {
        if (!h.onRejected) {
          h.reject(this._value);
        } else {
          const result = h.onRejected(this._value);
          h.resolve(result);
        }
      }
    } catch (err) {
      h.reject(err);
    }
  }

  then<TResult1 = T, TResult2 = never>(
    onFulfilled?: ((value: T) => TResult1 | PromiseLike<TResult1>) | null,
    onRejected?: ((reason: any) => TResult2 | PromiseLike<TResult2>) | null,
  ): PromisePolyfill<TResult1 | TResult2> {
    return new PromisePolyfill<TResult1 | TResult2>((resolve, reject) => {
      const handler = {
        onFulfilled:
          typeof onFulfilled === "function" ? onFulfilled : undefined,
        onRejected: typeof onRejected === "function" ? onRejected : undefined,
        resolve,
        reject,
      };

      if (this._state === State.Pending) {
        this._handlers.push(handler);
      } else {
        enqueueMicrotask(() => this._callHandler(handler));
      }
    });
  }

  catch<U = never>(onRejected?: OnRejected<U>): PromisePolyfill<U | T> {
    return this.then(undefined, onRejected as any);
  }

  finally(onFinally?: () => void): PromisePolyfill<T> {
    return this.then(
      (value) => {
        if (typeof onFinally === "function") onFinally();
        return value;
      },
      (reason) => {
        if (typeof onFinally === "function") onFinally();
        throw reason;
      },
    );
  }

  static resolve<T>(value?: T | PromiseLike<T>): PromisePolyfill<T> {
    if (value instanceof PromisePolyfill) return value;
    return new PromisePolyfill<T>((resolve) => resolve(value as any));
  }

  static reject<T = never>(reason?: any): PromisePolyfill<T> {
    return new PromisePolyfill<T>((_, reject) => reject(reason));
  }

  static all<T>(iterable: (T | PromiseLike<T>)[]): PromisePolyfill<T[]> {
    return new PromisePolyfill<T[]>((resolve, reject) => {
      if (!Array.isArray(iterable))
        return reject(new TypeError("Promise.all expects an array"));
      const results: T[] = [] as any;
      let remaining = iterable.length;
      if (remaining === 0) return resolve(results);
      iterable.forEach((item, i) => {
        PromisePolyfill.resolve(item).then((val) => {
          results[i] = val;
          remaining -= 1;
          if (remaining === 0) resolve(results);
        }, reject);
      });
    });
  }

  static race<T>(iterable: (T | PromiseLike<T>)[]): PromisePolyfill<T> {
    return new PromisePolyfill<T>((resolve, reject) => {
      if (!Array.isArray(iterable))
        return reject(new TypeError("Promise.race expects an array"));
      for (const item of iterable) {
        PromisePolyfill.resolve(item).then(resolve, reject);
      }
    });
  }

  static allSettled<T>(
    iterable: (T | PromiseLike<T>)[],
  ): PromisePolyfill<
    Array<
      { status: "fulfilled"; value: T } | { status: "rejected"; reason: any }
    >
  > {
    return new PromisePolyfill((resolve, _reject) => {
      if (!Array.isArray(iterable))
        throw new TypeError("Promise.allSettled expects an array");
      const results: any[] = [];
      let remaining = iterable.length;
      if (remaining === 0) return resolve(results);
      iterable.forEach((item, i) => {
        PromisePolyfill.resolve(item).then(
          (v) => {
            results[i] = { status: "fulfilled", value: v };
            remaining -= 1;
            if (remaining === 0) resolve(results);
          },
          (r) => {
            results[i] = { status: "rejected", reason: r };
            remaining -= 1;
            if (remaining === 0) resolve(results);
          },
        );
      });
    });
  }

  get [Symbol.toStringTag]() {
    return "Promise";
  }
}

export const PromiseShim = PromisePolyfill as unknown as PromiseConstructor;
