ws = new WebSocket("ws://127.0.0.1:9002");
document.getElementById("send").onclick = function(){
  const msg = {
    position : [parseInt(document.getElementById("x").value), parseInt(document.getElementById("y").value)],
    color : parseInt(document.getElementById("color").value)
  };
  ws.send(JSON.stringify(msg));
  document.getElementById("x").value = "";
  document.getElementById("y").value = "";
  document.getElementById("color").value = "";
};
ws.onmessage = (event) => {
  data = JSON.parse(event.data);
  if (data["position"].length == 0) {
    alert(data["color"] + " has won!");
  }
};
