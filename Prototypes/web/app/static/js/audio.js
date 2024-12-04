export class AudioHandler {
    /**
     * Oppretter en ny AudioHandler.
     *
     * @constructor
     */
    constructor() {
        this.audiocontext = new AudioContext();
        this.audioqueue = [];
        this.isplaying = false;
    }

    addchunk(chunk) {
        const arraybuffer = new Uint8Array(chunk).buffer;

        this.audiocontext.decodeAudioData(arraybuffer, buffer => {
            this.audioqueue.push(buffer);
            if (!this.isplaying) {
                this.playnextchunk();
            }
        }, error => {
            console.error("decode error ", error);
        });
    }
    
    playnextchunk() {
        if (this.audioqueue.length > 0) {
            const source = this.audiocontext.createBufferSource();
            source.buffer = this.audioqueue.shift();
            source.connect(this.audiocontext.destination);

            this.isplaying = true;

            source.start();

            source.onended = () => {
                if (this.audioqueue.length > 0) {
                    this.playnextchunk();
                } else {
                    this.isplaying = false;
                }
            };
        }
    }

    clearqueue() {
        this.audioqueue = [];
        this.isplaying = false;
    }

    /**
     * Avspilling av lydfil mottatt fra server
     *
     * @param {ArrayBuffer} arraybuffer Rå lyd-data
     */
    playfile(arraybuffer) {
        this.audiocontext.decodeAudioData(arraybuffer, (buffer) => {
            const source = this.audiocontext.createBufferSource();
            source.buffer = buffer;
            source.connect(this.audiocontext.destination);
            source.start();
            console.log("playing audio");
        }, (error) => {
            console.error("failed to decode exit with error:", error);
        });
    }
}