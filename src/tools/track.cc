// Copyright (c) 2007, 2008 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <algorithm>
#include <string>
#include <vector>

#include "libmv/base/scoped_ptr.h"
#include "libmv/correspondence/matches.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_pyramid.h"
#include "libmv/image/image_sequence_io.h"
#include "libmv/image/cached_image_sequence.h"
#include "libmv/image/pyramid_sequence.h"
#include "third_party/gflags/gflags/gflags.h"

DEFINE_bool(debug_images, true, "Output debug images.");
DEFINE_double(sigma, 0.9, "Blur filter strength.");
DEFINE_int32(pyramid_levels, 4, "Number of levels in the image pyramid.");

using namespace libmv;

using std::sort;
using std::string;

int main(int argc, char **argv) {
  gflags::SetUsageMessage("Track a sequence.");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  // This is not the place for this. I am experimenting with what sort of API
  // will be convenient for the tracking base classes.
  std::vector<string> files;
  for (int i = 1; i < argc; ++i) {
    files.push_back(argv[i]);
  }
  sort(files.begin(), files.end());

  if (files.size() < 2) {
    printf("Not enough files.\n");
    return 1;
  }

  ImageCache cache;
  scoped_ptr<ImageSequence> source(ImageSequenceFromFiles(files, &cache));
  PyramidSequence *pyramid_sequence =
      MakePyramidSequence(source.get(), FLAGS_pyramid_levels, FLAGS_sigma);

  KLTContext klt;
  Matches matches;

  scoped_ptr<ImagePyramid> pyramid(pyramid_sequence->Pyramid(0));
  KLTContext::FeatureList features;
  klt.DetectGoodFeatures(pyramid->Level(0), &features);
  int i = 0;
  for (KLTContext::FeatureList::iterator it = features.begin();
       it != features.end(); ++it, ++i) {
    matches.Insert(0, i, *it);
  }

  for (size_t i = 1; i < files.size(); ++i) {
    printf("Tracking %2zd features in %s\n", features.size(), files[i].c_str());

    for (Matches::Features<KLTPointFeature> r =
         matches.InImage<KLTPointFeature>(i-1); r; ++r) {
      KLTPointFeature *next_position = new KLTPointFeature;
      if (klt.TrackFeature(pyramid_sequence->Pyramid(i-1), *r.feature(),
                           pyramid_sequence->Pyramid(i), next_position)) {
        matches.Insert(i, r.track(), next_position);
      } else {
        delete next_position;
      }
    }
  }

  // XXX
  // TODO(keir): Now do something useful with 'correspondences'!
  // XXX
  //
  printf( "\n %2d tracks found\n", (int)matches.NumTracks());
  return 0;
}


