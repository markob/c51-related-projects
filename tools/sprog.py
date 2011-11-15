import parallel

class LPT2SPI(object):
	
	def enableDevice(self):
		self.setRST(True)
	
	def disableDevice(self):
		self.setRST(False)
	
	def readByte(self):
		byte = 0
		
		for i in xrange(0, 8):
			byte = (byte<<1) | self.readBit()
		
		return byte
	
	def readBit(self):
		pass

lpt = parallel.Parallel()

# set RST first
lpt.setInSelect(1)

# do device initialization
lpt.
