const WS_URI = 'ws://ec2-43-204-229-175.ap-south-1.compute.amazonaws.com:8081';
const ws = new WebSocket(WS_URI);

function blink() {
  ws.send('blink');
}

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
