import { MazeMap } from './mmap.js';
import { AudioHandler } from './audio.js';


const audio = new AudioHandler();
let socket;

/**
 * Sørger for at nettleseren oppretter en ``cookie`` før websocket starter.
 *
 * @function fetch
 */
fetch('/set_cookie')
    .then(response => {
        if (response.ok) {
            console.log("cookie set");
            startwebsocket();
        } else {
            console.error("failed cookie");
        }
    })
    .catch(error => console.error("Error setting cookie:", error));

/**
 * Starter WebSocket-tilkoblingen og setter opp hendelseslyttere for ulike servermeldinger.
 *
 * @function startwebsocket
 */
function startwebsocket() {
    socket = io("https://gruppe1.tech", {
        path: "/socket.io/",
        transports: ["websocket"],
    });

    socket.on('connect', connectsocket);
    socket.on('cookie_update', updatecookie);
    socket.on('cookie_valid', confirmcookie);
    socket.on('mqttsocket', mqttreceiver);
    socket.on('test_event', receivetest);
    socket.on('colorchange', colorhandler);
    socket.on('getlocation', locationhandler);
    socket.on('another_event', anothereventhandler);
    socket.on('audio_file', audiofilehandler);

    socket.onAny(loganyevent);
}

/**
 * Håndterer ``connect``
 *
 * @function connectsocket
 * @returns {void}
 */
function connectsocket() {
    console.log("Connected");
    socket.emit("ready_for_message");
}

/**
 * Håndterer ``cookie_update`` fra Flask socket
 *
 * @function updatecookie
 * @param {Object} data - Data relatert til oppdaterte cookies.
 * @returns {void}
 */
function updatecookie(data) {
    console.log("New cookie generated for:", data.user_data);
}

/**
 * Håndterer ``cookie_valid``.
 *
 * @function confirmcookie
 * @param {Object} data - Data om gyldige cookies.
 * @returns {void}
 */
function confirmcookie(data) {
    console.log("Cookie already exists:", data.user_data);
}

/**
 * Håndterer ``mqttsocket``.
 *
 * @function mqttreceiver
 * @param {Object} data - Data fra MQTT-socket.
 * @returns {void}
 */
function mqttreceiver(data) {
    console.log("Updated info: ", data);
    showinfo(data);
}

function receivetest(data) {
    console.log("Test event received:", data);
}

/**
 * Sørger for at ``colorchange``-hendelse på socket blir oppfylt.
 *
 * @function colorhandler
 * @param {Object} data - Data som inneholder fargeinformasjon.
 */
function colorhandler(data) {
    if (data) {
        document.documentElement.style.setProperty('--common-color', data.color);
        console.log(`Changed header color to ${data.color}`);
    } else {
        console.error("Problem with colorchange socket");
    }
}

/**
 * Sørger for at informasjon over WebSocket-en ``getlocation`` behandles korrekt.
 * 
 * Funksjonen undersøker først om den har mottatt en gyldig POI (Point of Interest).
 * Deretter kaller den det eksterne biblioteket ``MazeMap`` med POI. 
 * Til slutt returnerer informasjonen til funksjonen `handlepoi()`
 *
 * @function locationhandler
 * @param {Object} data - Data som inneholder POI og enhets-ID.
 * @param {string} data.poiID - ID for punktet av interesse.
 * @param {string} data.device - Enhets-ID assosiert med forespørselen.
 */
function locationhandler(data) {
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
}

/**
 * Just for testing not important
 *
 */
function anothereventhandler(data) {
    console.log("Another test from landing.py", data);
}

/**
 * Håndterer 'audio_file'-hendelsen.
 *
 * @function audiofilehandler
 * @param {ArrayBuffer} audiofile Rå lyd-data.
 */
function audiofilehandler(audiofile) {
    audio.playfile(audiofile);
    console.log("Playing audio file");
}

/**
 * Logger alle hendelser over SocketIO-tilkoblingen.
 *
 * @function loganyevent
 * @param {string} event Alle hendelser
 * @param {Object} data Data sendt over socket
 */
function loganyevent(event, data) {
    console.log(`Received event: ${event}`, data);
}

/**
 * Loggfører beskjeder fra MQTT i nettleservinduet.
 *
 * @function showinfo
 * @param {Object} data - Data mottatt fra MQTT.
 */
function showinfo(data) {
    const mqttList = document.getElementById("mqtt-data");

    const mqttItem = document.createElement("li");

    mqttItem.innerHTML = `<strong> ${data.topic}: </strong> message: ${data.message} device-id: ${data.device}`;

    mqttList.appendChild(mqttItem);
}

/**
 * Håndterer ``strength``-knappen i nettleseren.
 *
 * @function handlestrengthbutton
 */
function handlestrengthbutton() {
    const collectedvalue = document.getElementById("strength").value;
    const savedeviceid = localStorage.getItem("chosendevice");


    if (isNaN(parseInt(collectedvalue)) || collectedvalue === "") {
        console.log("Please input valid numeric value");
        return;
    }

    if (!savedeviceid) {
        console.log("no savedevice selected");
        return;
    }

    fetch('/front/', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({strength: collectedvalue, device: savedeviceid})
    })
    .then(response => response.json())
    .then(data => console.log(data))
    .catch(error => console.log('Errors:', error));
}

window.handlestrengthbutton = handlestrengthbutton;

/**
 * Håndterer lydavspilling og vising av informasjon innhentet fra MazeMap.
 *
 * @function handlepoi
 * @param {Object} data - Data mottatt fra MQTT.
 */
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

/**
 * Håndterer klikk på knappen med ID `deviceselectbutton`.
 * 
 * Når knappen trykkes:
 * 
 * - `toggler` synligheten til enhetinput-feltet.
 *
 * @function deviceselectclick
 */
document.getElementById("deviceselectbutton").addEventListener("click", function () {
    const devicewrapper = document.getElementById("devicewrapper");
    const deviceidinput = document.getElementById("deviceidinput");

    devicewrapper.classList.toggle("visible");

    if (devicewrapper.classList.contains("visible")) {
        deviceidinput.focus();
    }
});


/**
 * Lagrer hvilken enhet styrke-informasjonen skal returneres til.
 *
 * @function savedevice
 * @param {Object} data - Data mottatt fra MQTT.
 */
function savedevice() {
    const deviceidinput = document.getElementById("deviceidinput").value.trim();

    if (deviceidinput === "") {
        console.error("select valid id");
        return;
    }

    document.getElementById("chosendevice").textContent = `Chosen device: ${deviceidinput}`;

    localStorage.setItem("chosendevice", deviceidinput);

    document.getElementById("devicewrapper").classList.remove("visible");
    console.log(`device id set: ${deviceidinput}`);
}

/**
 * 
 * Aktiverer lagring til enhet ved muse-klikk.
 *
 * @function savedevicebutton
 */
document.getElementById("savedevicebutton").addEventListener("click", function () {
    savedevice();
});

/**
 * Håndterer lagring av enhet
 *
 * @function deviceinput
 */
document.getElementById("deviceidinput").addEventListener("keydown", function(event) {
    if (event.key === "Enter") {
        event.preventDefault();
        savedevice();
        const strengthinput = document.getElementById("strength");
        strengthinput.focus();
    }
});

/**
 * Forteller funksjonen `handlestrengthbutton()` til å returnere når enter-tasten trykkes.
 *
 * @function keydownlistener
 */
document.getElementById("strength").addEventListener("keydown", function(event) {
    if (event.key === "Enter") {
        event.preventDefault();
        handlestrengthbutton();
        this.blur();
    }
});


/**
 * Kjører når vinudet i nettleseren lastes inn
 *
 * @function onload
 */
window.onload = function () {
    const savedeviceid = localStorage.getItem("chosendevice");
    if (savedeviceid) {
        document.getElementById("chosendevice").textContent = `Chosen device: ${savedeviceid}`;
        document.getElementById("deviceidinput").value = savedeviceid;
    }
};

