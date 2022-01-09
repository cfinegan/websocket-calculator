import WebSocket from "ws";

// first 2 arguments will always be 'path/to/node' and 'main.js'
const args = process.argv.slice(2);

function usage() {
  console.error("usage: node path/to/main.js <type> <a> <b>");
  console.error("<type> must be 'add' or 'subtract'");
  console.error("<a> and <b> must be numbers");
}

// entry point
(function() {
  if (args.length < 3) {
    usage();
    return;
  }

  const type = args[0];
  const aStr = args[1]
  const bStr = args[2];

  const a = Number(aStr);
  const b = Number(bStr);

  if (type !== "add" && type !== "subtract") {
    console.error("Error! Invalid message type: " + type);
    usage();
    return;
  }
  if (isNaN(a)) {
    console.error("Error! Invalid number for 'a': " + aStr);
    usage();
    return;
  }
  if (isNaN(b)) {
    console.error("Error! Invalid number for 'b': " + bStr);
    usage();
    return;
  }

  const message = JSON.stringify({
    type: type,
    payload: {
      a: a,
      b: b
    }
  });

  const connection = new WebSocket("ws://localhost:8080");

  connection.onopen = function() {
    connection.send(message);
  };

  connection.onerror = function(error) {
    console.error("websocket error: " + error);
  };

  connection.onmessage = function(event) {
    console.log(event.data);
    connection.close();
  };
}());
