const WS_URI = 'ws://192.168.74.115:81';
const ws = new WebSocket(WS_URI);

ws.addEventListener('open', (event) => {
  ws.send(
    JSON.stringify({
      client: '81',
      operation: 'connecting',
      data: {},
    })
  );
});

ws.onmessage = (message) => {
  let md = JSON.parse(message.data);
  console.log(md);
  document.querySelector('#image').src = 'data:image/jpeg;base64,' + md.image;
};
