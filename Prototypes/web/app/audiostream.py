from pydub import AudioSegment
from io import BytesIO
import time
from utils.logging import logger

# TODO: implement streaming

class AudioStream:
    """
    Funksjonalitet:
        - Leser inn en lydfil i opus-format.
        - Sender en lydfil i rå-data over socket til WebServer.

    :param str file_path: Stien til filen som skal spilles av.

    :ivar str filepath: Stien til lydfilen.


    """
    def __init__(self, file_path, chunklength=0.05):
        self.filepath = file_path
        self.chunklength = chunklength
        #self.samplerate = None
        self.chunksize = None
        self.samples = None
        self.position = 0

    # currently not in use
    def init_stream(self):
        """
        Oppretter en forbindelse med stream

        """
        logger.info("AudioStream: Started init_stream")
        try:
            with open(self.filepath, "rb") as f:
                opusfiledata = f.read()
                logger.info(f"AudioStream: Read {len(opusfiledata)} bytes from {self.filepath}")

            opus_data = BytesIO(opusfiledata)
            logger.info("AudioStream: Created BytesIO object")

            self.samples = AudioSegment.from_file(opus_data, codec="opus")
            logger.info("AudioStream: Successfully loaded AudioSegment")

            self.chunksize = int(self.chunklength * 1000)
            self.position = 0
            logger.info(f"AudioStream: Initialized with chunksize={self.chunksize}")

        except Exception as e:
            logger.error(f"AudioStream: Failed to initialize stream. Error: {e}")
            raise

    # currently not in use
    def get_next_chunk(self):
        """
        Henter nye chunks

        """
        logger.info("AudioStream: Getting next chunk")
        # chunk = self.samples.readframes(self.chunksize)

        begin = self.position
        end = begin + self.chunksize
        
        chunk = self.samples[begin:end]

        if len(chunk) == 0:
            return None

        self.position = end
        return chunk.raw_data
    
    # currently not in use
    def stream_chunks(self):
        logger.info("AudioStream: stream_chunks called")
        self.init_stream()
        while True:
            chunk = self.get_next_chunk()
            if chunk is None:
                logger.info("AudioStream: get_next_chunk returned None, ending stream")
                break
            logger.info(f"AudioStream: yielding chunk of size {len(chunk)} bytes")
            yield chunk

    def simple_send(self):
        """
        Sender lydfil som rå-data til en WebServer.
        


        Returns:
            bytes: Data fra lydfilen som sendes til serveren.

        Raises:
            Exception: Hvis filen ikke kan leses eller en annen feil oppstår.
        """
        logger.info("AudioStream: sending simple file")
        try:
            with open(self.filepath, "rb") as f:
                opusfiledata = f.read()
                logger.info(f"AudioStream: Read {len(opusfiledata)} bytes from {self.filepath}")
                return opusfiledata
        except Exception as e:
            logger.error(f"AudioStream: failed to send simple file: {e}")
            raise