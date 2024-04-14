ws = new WebSocket("ws://192.168.0.248:9002");
document.getElementById("send").onclick = function(){
  const msg = {
    position : [parseInt(document.getElementById("x").value), parseInt(document.getElementById("y").value)],
    color : parseInt(document.getElementById("color").value)
  };
  ws.send(JSON.stringify(msg));
  document.getElementById("x").value = "";
  document.getElementById("y").value = "";
};
ws.onmessage = (event) => {
  data = JSON.parse(event.data);
    if (data["position"].length == 0) {
        alert(data["color"] + " has won!");
    } else {
        p = data["position"];
        c = data["color"];
        var tbodyRef = document.getElementById('history').getElementsByTagName('tbody')[0];
        var row = tbodyRef.insertRow();
        let c0 = row.insertCell(0);
        let c1 = row.insertCell(1);
        let c2 = row.insertCell(2);
        c0.innerHTML = c;
        c1.innerHTML = p[0];
        c2.innerHTML = p[1];
    }
};
