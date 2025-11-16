#!/usr/bin/env python3
"""
ColumnDB Setup Script - Quick setup for UV or traditional pip
"""

import subprocess
import sys
import platform

def run_command(cmd, description):
    """Run a command and report status"""
    print(f"\n{'='*60}")
    print(f"📌 {description}")
    print(f"{'='*60}")
    print(f"Running: {cmd}\n")
    
    result = subprocess.run(cmd, shell=True)
    if result.returncode != 0:
        print(f"\n❌ Failed: {description}")
        return False
    else:
        print(f"\n✅ Success: {description}")
        return True

def check_uv_installed():
    """Check if UV is installed"""
    result = subprocess.run(
        "uv --version" if sys.platform != "win32" else "uv.exe --version",
        shell=True,
        capture_output=True
    )
    return result.returncode == 0

def setup_with_uv():
    """Setup using UV"""
    print("\n🚀 Setting up ColumnDB with UV...")
    
    steps = [
        ("python setup.py build_ext --inplace", "Building C extension"),
        ("uv pip install -e .", "Installing ColumnDB package"),
        ("uv pip install -e \".[dev,pandas]\"", "Installing dev dependencies"),
    ]
    
    for cmd, desc in steps:
        if not run_command(cmd, desc):
            return False
    
    print("\n" + "="*60)
    print("🎉 Setup complete with UV!")
    print("="*60)
    print("\nNext steps:")
    print("  1. Run example:     uv run python examples/basic_usage.py")
    print("  2. Run tests:       uv run pytest tests/ -v")
    print("  3. Format code:     uv run black columndb/ tests/ examples/")
    print("  4. Lint code:       uv run flake8 columndb/ tests/ examples/")
    print("\nFor more info: see UV_GUIDE.md")
    
    return True

def setup_with_pip():
    """Setup using traditional pip"""
    print("\n🚀 Setting up ColumnDB with pip...")
    
    steps = [
        ("python setup.py build_ext --inplace", "Building C extension"),
        ("pip install -e .", "Installing ColumnDB package"),
        ("pip install -e \".[dev,pandas]\"", "Installing dev dependencies"),
    ]
    
    for cmd, desc in steps:
        if not run_command(cmd, desc):
            return False
    
    print("\n" + "="*60)
    print("🎉 Setup complete with pip!")
    print("="*60)
    print("\nNext steps:")
    print("  1. Run example:     python examples/basic_usage.py")
    print("  2. Run tests:       python -m pytest tests/ -v")
    print("  3. Format code:     black columndb/ tests/ examples/")
    print("  4. Lint code:       flake8 columndb/ tests/ examples/")
    
    return True

def main():
    """Main setup routine"""
    print("\n" + "="*60)
    print("  ColumnDB - Setup Wizard")
    print("="*60)
    
    print("\nChoose your setup method:")
    print("  1. UV (faster, recommended)     ⚡")
    print("  2. pip (traditional)           📦")
    print("  3. Skip setup                  ⏭️")
    
    choice = input("\nEnter choice (1-3): ").strip()
    
    if choice == "1":
        if not check_uv_installed():
            print("\n❌ UV is not installed!")
            print("Install UV with: pip install uv")
            print("Or visit: https://docs.astral.sh/uv/getting-started/")
            return False
        return setup_with_uv()
    
    elif choice == "2":
        return setup_with_pip()
    
    elif choice == "3":
        print("\n⏭️  Skipped setup.")
        print("To set up manually:")
        print("  1. Build:   python setup.py build_ext --inplace")
        print("  2. Install: pip install -e . (or uv pip install -e .)")
        return True
    
    else:
        print("\n❌ Invalid choice!")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
