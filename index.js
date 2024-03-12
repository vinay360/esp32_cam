const express = require('express');

const HOST = '0.0.0.0';
const HTTP_PORT = 3000;
const WS_PORT = 8081;
const CAM_PORT = 8000;

const app = express();
const WebSocket = require('ws');

let connectedClients = [];
let image = null;

let cam = null;

const clientWss = new WebSocket.Server({ port: WS_PORT, host: HOST }, () =>
  console.log(`WS Server running on port ${WS_PORT}`)
).on('connection', (ws) => {
  console.log('Client connected');
  ws.on('message', (data) => {
    console.log(data.toString());
    if (ws.readyState !== ws.OPEN) return;
    if (cam) cam.send(data);
  });
  connectedClients=[ws];
});

const camWss = new WebSocket.Server({ port: CAM_PORT, host: HOST }, () => {
  console.log(`CAM Server running on port ${CAM_PORT}`);
})
  .on('connection', (ws) => {
    console.log('CAM connected');
    cam = ws;
    ws.on('error', (err) => {
      console.log('err', err);
    });
    ws.on('message', (data) => {
      if (ws.readyState !== ws.OPEN) return;
      image = Buffer.from(Uint8Array.from(data)).toString('base64');
      connectedClients.forEach((client) => {
        if (client.readyState !== client.OPEN) return;
        client.send(JSON.stringify({ image }));
      });
    });
  })
  .on('error', (err) => {
    console.log(err);
  })
  .on('close', () => {
    console.log('closed');
  });
app.get('/index.html', (req, res) => {
  res.sendFile(__dirname + '/index.html');
});

app.get('/client.js', (req, res) => {
  res.sendFile(__dirname + '/client.js');
});

app.get('/', (req, res) => {
  res.send('OK');
});

app.listen(HTTP_PORT, HOST, () => {
  console.log(`HTTP Server running on port ${HTTP_PORT}`);
});
