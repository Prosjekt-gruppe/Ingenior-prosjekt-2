var socket = io();
socket.on('connect', function() {
    socket.emit('my event', {data: 'I\'m connected!'});
});

socket.on('mqttsocket', function(data) {
    console.log("Updated info: ", data);
    showInfo(data);
});

function showInfo(data) {
    const mqttList = document.getElementById("mqtt-data");

    const mqttItem = document.createElement("li");

    mqttItem.innerHTML = `<strong> ${data.topic}: </strong> message: ${data.message} device-id: ${data.device}`;

    mqttList.appendChild(mqttItem);
}