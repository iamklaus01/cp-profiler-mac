#include "layout_computer.hh"
#include "layout.hh"
#include "structure.hh"
#include "node_tree.hh"
#include "shape.hh"
#include "../utils/std_ext.hh"
#include "../utils/perf_helper.hh"

#include "cursors/layout_cursor.hh"
#include "cursors/nodevisitor.hh"

#include <QMutex>
#include <QDebug>
#include <thread>
#include <iostream>

namespace cpprofiler { namespace tree {

LayoutComputer::LayoutComputer(const NodeTree& tree, Layout& layout, const VisualFlags& nf)
: m_tree(tree), m_layout(layout), m_vis_flags(nf)
{

}

bool LayoutComputer::isDirty(NodeID nid) {
    return m_layout.isDirty(nid);
}

void LayoutComputer::setDirty(NodeID nid) {

    utils::DebugMutexLocker lock(&m_layout.getMutex());

    m_layout.setDirty(nid, true);
}


void LayoutComputer::dirtyUp(NodeID nid) {

    /// Is it necessary to have a tree mutex here?
    utils::DebugMutexLocker layout_lock(&m_layout.getMutex());

    while (nid != NodeID::NoNode && !m_layout.isDirty(nid)) {
        m_layout.setDirty(nid, true);
        nid = m_tree.getParent(nid);
    }

}


bool LayoutComputer::compute() {

    /// do nothing if there is no nodes

    if (m_tree.nodeCount() == 0) return false;

    static bool done = false;

    if (!done) {
        debug("thread") << "LayoutComputer:compute thread:" << std::this_thread::get_id() << std::endl;
        done = true;
    }

    /// TODO: come back here (ensure mutexes work correctly)
    utils::DebugMutexLocker tree_lock(&m_tree.treeMutex());
    utils::DebugMutexLocker layout_lock(&m_layout.getMutex());

    /// Ensures that sufficient memory is allocated for every node's shape
    m_layout.growDataStructures(m_tree.nodeCount());

    // perfHelper.begin("layout");
    LayoutCursor lc(m_tree.getRoot(), m_tree, m_vis_flags, m_layout);
    PostorderNodeVisitor<LayoutCursor>(lc).run();
    // perfHelper.end();

    // perfHelper.begin("layout: actually compute");

    // perfHelper.end();


    static int counter = 0;
    // std::cerr << "computed layout " << ++counter   << " times\n";

    return true;

}


}}