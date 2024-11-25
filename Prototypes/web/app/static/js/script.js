import { MazeMap } from './mmap.js';

var socket = io("https://gruppe1.tech", {
    path: "/socket.io/",
    transports: ["websocket"],
});

socket.on('connect', function() {
    socket.emit('my event', {data: 'socketio js connected using websockets'});
});

socket.on('mqttsocket', function(data) {
    console.log("Updated info: ", data);
    showInfo(data);
});


socket.on('getlocation', function(data) {
    const poi = data.poiID;

    if (poi) {
        console.log(`Found poi ${poi}`);    
        MazeMap.callPOI(data.poiID).then(poiInfo => {
            if (poiInfo) {
                console.log(`Bygning: ${poiInfo}`);
            } else {
                console.error("Mazemap could not find a valid POI");
            }
        }).catch(error => { console.log("mazemaperror: ", error) });
    } else {
        console.error("Wrong POI");
    }
});


function showInfo(data) {
    const mqttList = document.getElementById("mqtt-data");

    const mqttItem = document.createElement("li");

    mqttItem.innerHTML = `<strong> ${data.topic}: </strong> message: ${data.message} device-id: ${data.device}`;

    mqttList.appendChild(mqttItem);
}

function handlestrengthbutton() {
    const collectedvalue = document.getElementById("strength").value;

    if (isNaN(parseInt(collectedvalue)) || collectedvalue === "") {
        console.log("Please input valid numeric value");
        return;
    }

    fetch('/front/', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({strength: collectedvalue})
    })
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.log('Errors:', error));
}

