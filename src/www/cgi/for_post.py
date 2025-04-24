import sys
import os
import urllib.parse

def main():
	print('CONTENT_LENGTH', file=sys.stderr)
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	print(content_length, file=sys.stderr)
	name = os.environ.get('NAME', 0)
	print(name, file=sys.stderr)

	print(f"<p>Hello, {name}</p>")
	print(f"<p>Hello, {name}</p>")

if __name__ == '__main__':
	main()
else:
	print("Could not execut main of python script.")
