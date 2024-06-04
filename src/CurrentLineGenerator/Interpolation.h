#pragma once

#include <Geometry/Element.h>
#include <Geometry/CrossPoints.h>
#include <Geometry/Node.h>

#include <optional>

std::optional<CrossPoints> first_interpolation(const Element &element, Node &baseNode);

void second_interpolation(const CrossPoints &cross_points, Node &baseNode);

bool interpolation(const Element &element, Node &baseNode);

