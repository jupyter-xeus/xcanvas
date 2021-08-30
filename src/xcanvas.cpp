#include "xcanvas/xcanvas.hpp"

template class XCANVAS_API xw::xmaterialize<xc::xcanvas>;
template xw::xmaterialize<xc::xcanvas>::xmaterialize();
template class XCANVAS_API xw::xtransport<xw::xmaterialize<xc::xcanvas>>;
