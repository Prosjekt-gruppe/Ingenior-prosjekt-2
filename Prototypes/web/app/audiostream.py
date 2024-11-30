from pydub import AudioSegment
from io import BytesIO
import time

class AudioStream:
    def __init__(self, file_path, chunklength=0.05):
        self.filepath = file_path
        self.chunklength = chunklength
        #self.samplerate = None
        self.chunksize = None
        self.samples = None
        self.position = 0

    def init_stream(self):
        #self.wav_file = wave.open(self.filepath, 'rb')
        with open(self.filepath, "rb") as f:
            opusfiledata = f.read()

        opus_data = BytesIO(opusfiledata)
        
        self.samples = AudioSegment.from_file(opus_data, codec="opus")
        #self.samplerate = self.samples.frame_rate
        self.chunksize = int(self.chunklength * 1000)

    def get_next_chunk(self):
        # chunk = self.samples.readframes(self.chunksize)

        begin = self.position
        end = begin + self.chunksize
        
        chunk = self.samples[begin:end]

        if len(chunk) == 0:
            return None

        self.position = end
        return chunk.raw_data

    def stream_chunks(self):
        self.init_stream()
        while True:
            chunk = self.get_next_chunk()
            if chunk is None:
                break
            yield chunk