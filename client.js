const WS_URI = 'ws://3.111.213.252:8081';
const ws = new WebSocket(WS_URI);

function blink() {
  ws.send('blink');
}

ws.addEventListener('open', (event) => {
  // ws.send(
  //   JSON.stringify({
  //     client: '81',
  //     operation: 'connecting',
  //     data: {},
  //   })
  // );
});

ws.onmessage = (message) => {
  let md = JSON.parse(message.data);
  console.log(md);
  document.querySelector('#image').src = 'data:image/jpeg;base64,' + md.image;
};

function send(msg) {
  console.log(msg);
  if(ws.readyState !== ws.OPEN) {
    console.log("chutiye connect to kr");
    return;
  }
  ws.send(msg);
}