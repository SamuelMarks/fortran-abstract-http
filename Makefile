.PHONY: all build regen-ffi fmt fmt-check lint dialect-check doc-check test coverage check-all clean

BUILD_DIR ?= build
COV_BUILD_DIR ?= build-coverage

all: build

build:
	@if [ ! -f $(BUILD_DIR)/CMakeCache.txt ]; then \
		cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON -DFAH_BUILD_TESTING=ON; \
	fi
	cmake --build $(BUILD_DIR) -j

regen-ffi:
	sh scripts/regen-ffi.sh

fmt:
	find include src tests generated -maxdepth 3 \( -name '*.c' -o -name '*.h' \) -exec clang-format -i --style=LLVM {} +

fmt-check:
	find include src tests generated -maxdepth 3 \( -name '*.c' -o -name '*.h' \) -exec clang-format --dry-run --Werror --style=LLVM {} +

dialect-check:
	python3 tools/fah_dialect_lint.py --check-all

doc-check:
	mkdir -p docs/doxygen
	doxygen Doxyfile
	python3 tools/fah_doc_check.py --check-all

lint: fmt-check dialect-check doc-check
	python3 tools/fah_lint.py --check-all

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

coverage:
	sh scripts/run-coverage.sh $(COV_BUILD_DIR)

check-all: lint test coverage
	sh scripts/regen-ffi.sh
	git diff --exit-code generated/

clean:
	rm -rf $(BUILD_DIR) $(COV_BUILD_DIR) docs/doxygen
