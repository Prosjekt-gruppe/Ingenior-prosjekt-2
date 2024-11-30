export class AudioHandler {
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
}