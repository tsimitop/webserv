import sys
import os
import urllib.parse

def main():
	# print('CONTENT_LENGTH', file=sys.stderr)
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	# print(content_length, file=sys.stderr)
	name = os.environ.get('NAME', 0)
	# print(name, file=sys.stderr)

	# print("Content-Type: text/html\n")
	# print(f"Hello, {name}\n")
	# print("Hello, " + name + "\n")
	print(f"<p>Hello, {name}</p>")
	print(f"<p>Hello, {name}</p>")
	# print(f"<h1>Hello, {name}!</h1>")

if __name__ == '__main__':
	main()
else:
	print("Could not execut main of python script.")
