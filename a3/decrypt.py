#!/usr/bin/env python3
import base64
import sys
import requests

URL = "http://localhost:4555"
# read in a Base64-encoded cookie
cookie = sys.argv[1]
# decode this cookie
cookie = base64.b64decode(cookie)
# get every 16-byte blocks in cookie
block = [cookie[i:i+16] for i in range(0, len(cookie), 16)]

content = bytearray()

IV = bytearray(16)


for j, cookie in enumerate(block[1:]):
	# IV = bytearray(16)
	# get the decrypted ciphertext
	cb = bytearray(16)
	for i in range(15, -1, -1):
		# initialize responseCode to be 500(fail)
		responseCode = 500
		# while responseCode is not 200(ok)
		while responseCode != 200:
			IV[i] += 1
			encodeCookie = base64.b64encode(IV + cookie)
			# send encoded cookie to server
			response = requests.get(URL, cookies={'user': encodeCookie})
			# get response from server
			responseCode = response.status_code

		# if response is 200(ok)
		# padding starts with 0, followed by (n-1), (n-2), ... , 1
		cb[i] = IV[i] ^ 0
		IV[i] = cb[i] ^ (16-i-1)

	# perform CBC encryption
	content = bytearray(len(cb))
	for x in range(len(cb)):
		content[x] = bytearray(block[j])[x] ^ cb[x]
	content +=	content

# print out plaintext
print''.join(chr(x) for x in content)
