MATHVERSION = v4.5.0

default : math

math : math-release-$(MATHVERSION).zip
	unzip -q math-release-$(MATHVERSION).zip
	mv math-release-$(MATHVERSION) math
	$(MAKE) -f math/make/standalone math-libs

math-release-$(MATHVERSION).zip :
	curl -L -o math-release-$(MATHVERSION).zip "https://github.com/stan-dev/math/archive/refs/heads/release/$(MATHVERSION).zip"

clean :
	$(RM) -rf math

clean-all : clean
	$(RM) -rf math-release-$(MATHVERSION).zip

.PHONY : clean
