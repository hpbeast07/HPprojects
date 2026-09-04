#!/usr/bin/env python3
"""
Generate GLAD OpenGL 4.5 loader via glad2 pip package
Run: python generate_glad.py
"""
import subprocess
import sys
import os
import shutil

GLAD_OUT = os.path.join(os.path.dirname(__file__), "vendor", "glad_gen")
GLAD_INCLUDE = os.path.join(os.path.dirname(__file__), "vendor", "glad", "include")
GLAD_SRC     = os.path.join(os.path.dirname(__file__), "vendor", "glad", "src")

def run(cmd, **kwargs):
    print(f">> {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=True, text=True, **kwargs)
    if result.stdout: print(result.stdout)
    if result.stderr: print(result.stderr, file=sys.stderr)
    return result.returncode == 0

def main():
    print("=== GLAD2 OpenGL 4.5 Core Loader Generator ===")

    # Install glad2
    if not run([sys.executable, "-m", "pip", "install", "glad2", "--quiet"]):
        print("Failed to install glad2", file=sys.stderr)
        sys.exit(1)

    # Generate GLAD
    os.makedirs(GLAD_OUT, exist_ok=True)
    if not run([
        sys.executable, "-m", "glad",
        "--api", "gl:core=4.5",
        "--out-path", GLAD_OUT,
        "c",
        "--loader"
    ]):
        print("GLAD generation failed", file=sys.stderr)
        sys.exit(1)

    # Copy to vendor/glad
    include_src = os.path.join(GLAD_OUT, "include")
    src_src     = os.path.join(GLAD_OUT, "src")

    if os.path.exists(include_src):
        shutil.copytree(include_src, GLAD_INCLUDE, dirs_exist_ok=True)
        print(f"Copied includes to {GLAD_INCLUDE}")
    if os.path.exists(src_src):
        shutil.copytree(src_src, GLAD_SRC, dirs_exist_ok=True)
        print(f"Copied sources to {GLAD_SRC}")

    # Verify
    glad_h = os.path.join(GLAD_INCLUDE, "glad", "gl.h")
    glad_c = os.path.join(GLAD_SRC, "gl.c")
    khr_h  = os.path.join(GLAD_INCLUDE, "KHR", "khrplatform.h")

    ok = True
    for f in [glad_h, glad_c, khr_h]:
        if os.path.exists(f):
            size = os.path.getsize(f)
            print(f"  OK  {f} ({size} bytes)")
        else:
            print(f"  MISSING: {f}", file=sys.stderr)
            ok = False

    if ok:
        print("\nGLAD files generated successfully!")
    else:
        print("\nSome files missing!", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
