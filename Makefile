.PHONY: help build build-ext clean test install dev examples

help:
	@echo "ColumnDB - Python-first columnar database with C backend"
	@echo ""
	@echo "Available targets:"
	@echo "  build         - Build the C extension"
	@echo "  build-ext     - Build extension in-place"
	@echo "  install       - Install the package"
	@echo "  dev           - Install in development mode with dev dependencies"
	@echo "  clean         - Clean build artifacts"
	@echo "  test          - Run unit tests"
	@echo "  examples      - Run examples"
	@echo "  lint          - Run linting (black, flake8)"
	@echo "  format        - Format code with black"
	@echo "  help          - Show this help message"

build:
	python setup.py build

build-ext:
	python setup.py build_ext --inplace

install: build-ext
	pip install .

dev: build-ext
	pip install -e ".[dev,pandas]"

clean:
	rm -rf build/
	rm -rf dist/
	rm -rf *.egg-info/
	find . -type f -name '*.so' -delete
	find . -type f -name '*.o' -delete
	find . -type d -name '__pycache__' -exec rm -rf {} + 2>/dev/null || true
	find . -type f -name '*.pyc' -delete

test:
	python -m pytest tests/ -v

test-coverage:
	python -m pytest tests/ --cov=columndb --cov-report=html

examples: build-ext
	python examples/basic_usage.py

lint:
	black --check columndb/ tests/ examples/
	flake8 columndb/ tests/ examples/

format:
	black columndb/ tests/ examples/

.DEFAULT_GOAL := help
