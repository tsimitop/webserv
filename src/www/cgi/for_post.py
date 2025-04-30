import sys
import os
import urllib.parse

def main():
	# content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	name = os.environ.get('NAME', 0)
	print("""
	<html>
	<head>
		<title>CGI Retro Response</title>
		<link href=\"https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap\" rel="stylesheet\">
		<style>
			/* CRAZY RETRO STYLE */
			body, html {
				margin: 0;
				padding: 0;
				font-family: 'Press Start 2P', cursive;
				color: #00ffe0;
				background: #000;
				overflow-x: hidden;
				background-image: url(\'https://www.transparenttextures.com/patterns/pixel-weave.png\');
				background-size: 100px;
				animation: bgScroll 20s linear infinite;
			}

			@keyframes bgScroll {
				from { background-position: 0 0; }
				to { background-position: 1000px 0; }
			}

			body::before {
				content: \"\";
				position: fixed;
				top: 0;
				left: 0;
				height: 100%;
				width: 100%;
				background: linear-gradient(180deg, #2b1055, #7597de, #2b1055);
				z-index: -1;
				transform: scale(1.2);
				opacity: 0.2;
				animation: float 6s ease-in-out infinite;
			}

			@keyframes float {
				0%, 100% { transform: translateY(0) scale(1.2); }
				50% { transform: translateY(-10px) scale(1.25); }
			}

			h1, h2 {
				text-align: center;
				text-shadow: 0 0 5px #ff00ff, 0 0 10px #00ffff;
				animation: glitch 1s infinite;
			}

			@keyframes glitch {
				0% { text-shadow: 2px 0 red, -2px 0 cyan; }
				25% { text-shadow: -2px 0 red, 2px 0 cyan; }
				50% { text-shadow: 2px 2px red, -2px -2px cyan; }
				75% { text-shadow: -2px -2px red, 2px 2px cyan; }
				100% { text-shadow: 2px 0 red, -2px 0 cyan; }
			}

			input, button {
				display: block;
				margin: 10px auto;
				padding: 10px 20px;
				font-family: 'Press Start 2P', cursive;
				border: 3px dashed #00ffcc;
				background-color: #000;
				color: #00ffcc;
				cursor: pointer;
				transition: all 0.2s ease-in-out;
				text-transform: uppercase;
				box-shadow: 0 0 8px #00ffcc;
			}

			button:hover {
				background: #00ffcc;
				color: #000;
				box-shadow: 0 0 20px #00ffcc, 0 0 30px #00ffff;
				transform: scale(1.1) rotate(-1deg);
			}

			input::placeholder {
				color: #00ffccaa;
			}

			input[type="file"] {
				color: #00ffcc;
				background-color: #111;
				border: 2px solid #ff00ff;
			}

			p {
				text-align: center;
				font-size: 12px;
				color: #fff;
			}

			br {
				line-height: 2em;
			}

			#music-toggle {
				position: fixed;
				top: 20px;
				right: 20px;
				z-index: 999;
				font-size: 10px;
				background-color: #111;
				border: 2px dashed #ff00ff;
				color: #00ffcc;
				box-shadow: 0 0 8px #00ffff;
				padding: 10px 15px;
				transition: all 0.3s ease;
			}

			#music-toggle:hover {
				background-color: #00ffcc;
				color: #000;
				transform: scale(1.05);
				box-shadow: 0 0 12px #ff00ff;
			}
		</style>
	</head>
	<body>
		<h1>HEY!</h1>
	</body>
	</html>
	""")
	print(f"<p style=\"text-align: center; font-size: 12px; color: #fff;\">Once upon a time, there was a curious little program named {name}</p>")
	print(f"<p style=\"text-align: center; font-size: 12px; color: #fff;\">Thanks for visiting, {name}. Keep your fingers crossed for our hero's next big adventure!</p>")

if __name__ == '__main__':
	main()
else:
	print("Could not execut main of python script.")
