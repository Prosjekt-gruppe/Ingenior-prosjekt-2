import { MazeMap } from './mmap.js';
import { AudioHandler } from './audio.js';


const audio = new AudioHandler();
let socket;


fetch('/set_cookie')
    .then(response => {
        if (response.ok) {
            console.log("cookie set");
            startWebSocket();
        } else {
            console.error("failed cookie");
        }
    })
    .catch(error => console.error("Error setting cookie:", error));

function startWebSocket() {
    socket = io("https://gruppe1.tech", {
        path: "/socket.io/",
        transports: ["websocket"],
    });

    socket.on('connect', function() {
        console.log("Connected");
        socket.emit("ready_for_message");
    });

    socket.on('cookie_update', function (data) {
        console.log("New cookie generated for:", data.user_data);
    });

    socket.on('cookie_valid', function (data) {
        console.log("Cookie already exists:", data.user_data);
    });

    socket.on('mqttsocket', function(data) {
        console.log("Updated info: ", data);
        showInfo(data);
    });

    socket.on('test_event', function(data) {
        console.log("Test event received:", data);
    });

    socket.on('colorchange', function(data) {
        if (data) {
            document.documentElement.style.setProperty('--common-color', data.color);
            console.log(`Changed header color to ${data.color}`);
        } else {
            console.error("Problem with colorchange");
        }
    });

    socket.on('getlocation', function(data) {
        const poi = data.poiID;
        const deviceID = data.device;

        if (poi) {
            console.log(`Found POI ${poi}`);
            MazeMap.callPOI(poi).then(poiInfo => {
                if (poiInfo) {
                    console.log("Received:", poiInfo);
                    handlepoi(poiInfo, deviceID);
                } else {
                    console.error("MazeMap could not find a valid POI");
                }
            }).catch(error => {
                console.error("MazeMap error:", error);
            });
        } else {
            console.error("Wrong POI");
        }
    });

    socket.on('another_event', function(data) {
        console.log("Another test from landing.py", data);
    });

    // handle audio
    socket.on('audio_file', function(audiofile) {
        audio.playfile(audiofile)
        console.log("Playing audio file")
    });

    socket.onAny((event, data) => {
        console.log(`Received event: ${event}`, data);
    });
}
    
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

window.handlestrengthbutton = handlestrengthbutton;

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

    audio.clearqueue();

    socket.emit('request_audio', { poiId: poi.poiId });

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

document.getElementById("deviceselectbutton").addEventListener("click", function () {
    const devicewrapper = document.getElementById("devicewrapper");
    devicewrapper.classList.toggle("visible");
});

document.getElementById("savedevicebutton").addEventListener("click", function () {
    const deviceidinput = document.getElementById("deviceidinput").value.trim();

    if (deviceidinput === "") {
        console.error("select valid id");
        return;
    }

    document.getElementById("chosendevice").textContent = `Chosen device: ${deviceidinput}`;

    localStorage.setItem("chosendevice", deviceidinput);

    document.getElementById("devicewrapper").classList.remove("visible");
    console.log(`device id set: ${deviceidinput}`);
});

window.onload = function () {
    const savedDeviceId = localStorage.getItem("chosendevice");
    if (savedDeviceId) {
        document.getElementById("chosendevice").textContent = `Chosen device: ${savedDeviceId}`;
        document.getElementById("deviceidinput").value = savedDeviceId;
    }
};




