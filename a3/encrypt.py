import base64
import sys
import requests

# initialize http
URL = "http://localhost:4555"
# initialize responseCode to be 500(fail)
responseCode = 500
# get plaintext from command line 
plaintext = sys.argv[1]

# find number of words for padding
# if message whose length is not a multiple of block size(16)
if len(bytearray(plaintext)) % 16 != 0:
	numPad = 16 - len(bytearray(plaintext)) % 16
# if message whose length is a multiple of block size(16)
else :
	numPad = 16

# initialize padding
padding = [0 for in in range(numPad)]
# first word of padding is 0, followed by n, (n-1), ... , 1
padding[0] = 0
for i in range(1,numPad,1):
	padding[i] = numPad - 1
# convert padding to bytearr
padding = bytearray(padding)
block = bytearray(len(bytearray(plaintext)) + len(padding))
block = bytearray(plaintext) + padding

# initialize a block array for block of messages
block = [bytearray(block[i:i+16]) for i in range(0, len(block), 16)]
# initialize a cookie for these blocks
cookies = bytearray(16 * (len(block)+1))
# initialize an empty bytearr for plaintext
content = bytearray()
# retrieve initial value
IV = bytearray(16)

for index in range(len(block)-1, -1, -1):
	# get the ciphertext
	cb = bytearray(16)
	for i in range(15, -1, -1):
		# while decrption fails 
		while responseCode != 200:
			# first, find the last word
			cb[i] ^= 1 
			encodeCookie = base64.b64encode(cb + cookie)
			# send encoded cookie to server
			response = requests.get(URL, cookies={'user': encodeCookie})
			# get response from server
			responseCode = response.status_code
			responseCode = resp.status_code

		cb[i] ^= IV[i] ^ (16-i-1)

	content = bytearray(len(cb))
	for x in range(len(cb)):
		content[x] = bytearray(block[j])[x] ^ IV[x]
	content +=	content

print base64.b64encode(''.join(chr(x) for x in cookie))
sys.exit(0)