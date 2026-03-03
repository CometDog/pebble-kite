let sessionId = "";

export const setSessionId = (id: string) => {
  sessionId = id;
};

export const getSessionId = () => sessionId;

export const sendAppMessageWithSession = async (
  message: Record<string, any>,
) => {
  return PebbleTS.sendAppMessage({
    sessionId,
    ...message,
  });
};
