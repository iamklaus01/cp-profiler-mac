#pragma once

class QPainter;

namespace cpprofiler
{
namespace tree
{
namespace draw
{

void solution(QPainter &painter, int x, int y, bool selected);

void failure(QPainter &painter, int x, int y, bool selected);

void branch(QPainter &painter, int x, int y, bool selected);

void unexplored(QPainter &painter, int x, int y, bool selected);

void skipped(QPainter &painter, int x, int y, bool selected);

void pentagon(QPainter &painter, int x, int y, bool selected);
void big_pentagon(QPainter &painter, int x, int y, bool selected);

} // namespace draw
} // namespace tree
} // namespace cpprofiler