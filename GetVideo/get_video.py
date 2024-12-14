import requests
import sys


def download_video(u, path):
	res = requests.get(u, stream=True)
	if res.status_code == 200:
		with open(path, 'wb') as file:
			for chunk in res.iter_content(chunk_size=1024):
				if chunk:
					file.write(chunk)
		return True
	else:
		return False


def get_video_0():
	url = 'https://api.kuleu.com/api/MP4_xiaojiejie?type=json'
	response = requests.get(url)
	js = response.json()
	if js['code'] == 200:
		return download_video(js['mp4_video'], video_path)


if __name__ == "__main__":
	args = sys.argv
	video_path = args[1]
	while True:
		if get_video_0():
			break
