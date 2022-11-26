library("tools")

source("file_manipulation.R")

# Populates variables cache.dir, src.dir, test.dir and test.deps.dir,
# and the cache.dependencies function.
source("directories.R")

deps.dir <- "external"
if (!dir.exists(deps.dir)) {
  dir.create(deps.dir)
}

# Boost R Package Dependency and Testing
boost.cache.path <- cache.dependency(
  url = "https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz"
)
create_and_copy(
  from = dir(boost.cache.path, full.names = TRUE),
  to = file.path(deps.dir, "boost"),
  recursive = TRUE
)

