"""Fetch the pinned, public course video using Python's standard library."""
import hashlib
from pathlib import Path
import shutil
import urllib.request

COMMIT = "2bb6179ef9900bd1e5e815c287994d7357e62fe4"
URL = f"https://raw.githubusercontent.com/TongjiSuperPower/sp_vision_tutorial_26/{COMMIT}/lecture4/class/video.avi"
SHA256 = "70d04ce4a73197ee1d5e0e771c148f8f883866bc8a3a9dee86125dffde06533b"


def digest(path):
    hasher = hashlib.sha256()
    with path.open("rb") as stream:
        while chunk := stream.read(1024 * 1024):
            hasher.update(chunk)
    return hasher.hexdigest()


def main():
    target = Path(__file__).resolve().parents[1] / "data" / "video.avi"
    target.parent.mkdir(parents=True, exist_ok=True)
    if target.exists():
        if digest(target) != SHA256:
            raise RuntimeError(f"Existing video differs; refusing to overwrite: {target}")
        print(f"Course video verified: {target}")
        return
    temporary = target.with_suffix(".avi.part")
    local_course_copy = Path("/mnt/e/robomaster/school_training/hello_armor/video.avi")
    try:
        if local_course_copy.is_file() and digest(local_course_copy) == SHA256:
            with local_course_copy.open("rb") as source, temporary.open("xb") as output:
                shutil.copyfileobj(source, output)
        else:
            request = urllib.request.Request(URL, headers={"User-Agent": "RM-Task4-Course-Sample"})
            with urllib.request.urlopen(request, timeout=60) as source, temporary.open("xb") as output:
                shutil.copyfileobj(source, output)
        if digest(temporary) != SHA256:
            raise RuntimeError("Video checksum mismatch.")
        temporary.rename(target)
    finally:
        if temporary.exists():
            temporary.unlink()
    print(f"Course video ready: {target}")


if __name__ == "__main__":
    main()
