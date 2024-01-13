const express = require('express');

const HOST = '192.168.74.115';
const HTTP_PORT = 80;
const WS_PORT = 81;
const CAM_PORT = 8000;

const app = express();
const WebSocket = require('ws');

let connectedClients = [];
let image = null;

const wss = new WebSocket.Server({ port: WS_PORT, host: HOST }, () =>
  console.log(`WS Server running on port ${WS_PORT}`)
).on('connection', (ws) => {
  console.log('Client connected');
  ws.on('message', (data) => {
    if (ws.readyState !== ws.OPEN) return;
    console.log(JSON.parse(data));
    connectedClients.push(ws);
  });
});

const camWss = new WebSocket.Server({ port: CAM_PORT, host: HOST }, () => {
  console.log(`CAM Server running on port ${CAM_PORT}`);
})
  .on('connection', (ws) => {
    ws.onopen = (event) => {
      console.log('CAM open');
    };
    ws.on('', (req) => {
      console.log(req);
    });
    ws.on('error', (err) => {
      console.log('err', err);
    });
    ws.on('open', (event) => {
      console.log('CAM connected');
    });
    ws.on('message', (data) => {
      console.log('CAM message');
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
