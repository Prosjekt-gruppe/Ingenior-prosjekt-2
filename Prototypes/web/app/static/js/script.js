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
    const deviceID = data.device

    if (poi) {
        console.log(`Found poi ${poi}`);    
        MazeMap.callPOI(poi).then(poiInfo => {
            if (poiInfo) {
                console.log("Received:", poiInfo);
                handlepoi(poiInfo, deviceID);
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

function handlepoi(poi, deviceID) {
    const poiList = document.getElementById("poi-data");

    const poiItem = document.createElement("li");
    const names = poi.names && Array.isArray(poi.names) ? poi.names.join(", ") : "Unknown Name";

    
    poiItem.innerHTML = `
        <strong>POI ID:</strong> ${poi.poiId}<br>
        <strong>Names:</strong> ${names}<br>
        <strong>Floor:</strong> ${poi.floorName}<br>
        <strong>Building:</strong> ${poi.buildingName}
    `;
    poiList.appendChild(poiItem);


    fetch('/mqtt/returndata', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
            ...poi,
            deviceID: deviceID
        })
    })
    .then(response => response.json())
    .then(data => console.log("Server response:", data))
    .catch(error => console.log('Errors:', error));

}

