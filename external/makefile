MATHVERSION = v4.5.0

math/stan/math.hpp : math-release-$(MATHVERSION).zip
	unzip math-release-$(MATHVERSION).zip
	mv math-release-$(MATHVERSION) math
	$(MAKE) -f math/make/standalone math-libs

math-release-$(MATHVERSION).zip :
	curl -L -o math-release-$(MATHVERSION).zip "https://github.com/stan-dev/math/archive/refs/heads/release/$(MATHVERSION).zip"
