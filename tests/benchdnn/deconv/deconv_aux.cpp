/*******************************************************************************
* Copyright 2017-2022 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include "dnn_types.hpp"
#include "dnnl_common.hpp"

#include "deconv/deconv.hpp"

namespace deconv {

alg_t str2alg(const char *str) {
#define CASE(_alg) \
    if (!strcasecmp(STRINGIFY(_alg), str)) return _alg
    CASE(DIRECT);
    CASE(deconvolution_direct);
    CASE(WINO);
    CASE(deconvolution_wino);
#undef CASE
    assert(!"unknown algorithm");
    return UNDEF;
}

const char *alg2str(alg_t alg) {
    if (alg == DIRECT) return "direct";
    if (alg == WINO) return "wino";
    assert(!"unknown algorithm");
    return "undef";
}

alg_t alg_kind2alg(dnnl_alg_kind_t alg) {
    if (alg == dnnl_deconvolution_direct) return DIRECT;
    if (alg == dnnl_deconvolution_winograd) return WINO;
    assert(!"unknown algorithm");
    return DIRECT;
}

int str2desc(desc_t *desc, const char *str) {
    /* canonical form:
     * gXmbX_icXidXihXiwX_ocXodXohXowX_kdXkhXkwX_sdXshXswX_pdXphXpwX_ddXdhXdwXnS
     *
     * where X is number, S - string
     * note: symbol `_` is ignored
     *
     * implicit rules:
     *  - if smaller dimensions are not specified => square or cubic form;
     *  - if output is undefined => compute output;
     *  - if padding is undefined => compute trivial padding;
     */

    desc_t d {0};
    d.g = 1;
    d.mb = 2;
    d.sd = d.sh = d.sw = 1;
    d.pd = d.ph = d.pw = -1;

    const char *s = str;
    assert(s);

#define CASE_NN(prb, c) \
    do { \
        if (!strncmp(prb, s, strlen(prb))) { \
            ok = 1; \
            s += strlen(prb); \
            char *end_s; \
            d.c = strtol(s, &end_s, 10); \
            s += (end_s - s); \
            /* check any # groups, including one, works correctly */ \
            if (!strncmp(prb, "g", 1)) d.has_groups = true; \
            if (d.c < 0) return FAIL; \
            /* printf("@@@debug: %s: %d\n", prb, d. c); */ \
        } \
    } while (0)
#define CASE_N(c) CASE_NN(#c, c)
    while (*s) {
        int ok = 0;
        CASE_N(g);
        CASE_N(mb);
        CASE_N(ic);
        CASE_N(id);
        CASE_N(ih);
        CASE_N(iw);
        CASE_N(oc);
        CASE_N(od);
        CASE_N(oh);
        CASE_N(ow);
        CASE_N(kd);
        CASE_N(kh);
        CASE_N(kw);
        CASE_N(sd);
        CASE_N(sh);
        CASE_N(sw);
        CASE_N(pd);
        CASE_N(ph);
        CASE_N(pw);
        CASE_N(dd);
        CASE_N(dh);
        CASE_N(dw);
        if (*s == 'n') {
            d.name = s + 1;
            break;
        }
        if (*s == '_') ++s;
        if (!ok) return FAIL;
    }
#undef CASE_NN
#undef CASE_N

    if (d.has_groups && d.g <= 0) return FAIL;
    if (d.ic == 0 || d.oc == 0) return FAIL;
    if (d.sd <= 0 || d.sh <= 0 || d.sw <= 0) return FAIL;

    auto compute_out
            = [](int64_t i, int64_t k, int64_t s, int64_t p, int64_t d) {
                  return (i - 1) * s + (k - 1) * (d + 1) - 2 * p + 1;
              };
    auto compute_pad
            = [](int64_t o, int64_t i, int64_t k, int64_t s, int64_t d) {
                  return ((i - 1) * s - o + ((k - 1) * (d + 1) + 1)) / 2;
              };

    const bool no_d = (d.id | d.kd | d.od | d.dd) == 0 && d.sd == 1 && d.pd < 1;
    const bool no_h = (d.ih | d.kh | d.oh | d.dh) == 0 && d.sh == 1 && d.ph < 1;
    const bool no_w = (d.iw | d.kw | d.ow | d.dw) == 0 && d.sw == 1 && d.pw < 1;

    if (!no_d) {
        if (!d.id || !d.kd) return FAIL;
        if (!d.od) {
            if (d.pd < 0) d.pd = 0;
            d.od = compute_out(d.id, d.kd, d.sd, d.pd, d.dd);
            if (d.od <= 0) return FAIL;
        } else if (d.pd < 0)
            d.pd = compute_pad(d.od, d.id, d.kd, d.sd, d.dd);
    }

    if (!no_h) {
        if (!d.ih || !d.kh) return FAIL;
        if (!d.oh) {
            if (d.ph < 0) d.ph = 0;
            d.oh = compute_out(d.ih, d.kh, d.sh, d.ph, d.dh);
            if (d.oh <= 0) return FAIL;
        } else if (d.ph < 0)
            d.ph = compute_pad(d.oh, d.ih, d.kh, d.sh, d.dh);
    }

    if (!no_w) {
        if (!d.iw || !d.kw) return FAIL;
        if (!d.ow) {
            if (d.pw < 0) d.pw = 0;
            d.ow = compute_out(d.iw, d.kw, d.sw, d.pw, d.dw);
            if (d.ow <= 0) return FAIL;
        } else if (d.pw < 0)
            d.pw = compute_pad(d.ow, d.iw, d.kw, d.sw, d.dw);
    }

    if (sanitize_desc(d.ndims, {d.od, d.id, d.kd, d.sd, d.pd, d.dd},
                {d.oh, d.ih, d.kh, d.sh, d.ph, d.dh},
                {d.ow, d.iw, d.kw, d.sw, d.pw, d.dw}, {1, 1, 1, 1, 0, 0}, true)
            != OK)
        return FAIL;

    d.init_pad_r();
    *desc = d;

    return OK;
}

std::ostream &operator<<(std::ostream &s, const desc_t &d) {
    bool print_d = true, print_h = true, print_w = true;
    print_dhw(print_d, print_h, print_w, d.ndims,
            {d.od, d.id, d.kd, d.sd, d.pd, d.dd},
            {d.oh, d.ih, d.kh, d.sh, d.ph, d.dh},
            {d.ow, d.iw, d.kw, d.sw, d.pw, d.dw});

    auto print_spatial
            = [&](const char *d_str, int64_t d_val, const char *h_str,
                      int64_t h_val, const char *w_str, int64_t w_val) {
                  if (print_d) s << d_str << d_val;
                  if (print_h) s << h_str << h_val;
                  if (print_w) s << w_str << w_val;
              };

    if (canonical || d.has_groups) s << "g" << d.g;
    if (canonical || d.mb != 2) s << "mb" << d.mb;
    s << "ic" << d.ic;
    print_spatial("id", d.id, "ih", d.ih, "iw", d.iw);
    s << "oc" << d.oc;
    print_spatial("od", d.od, "oh", d.oh, "ow", d.ow);
    print_spatial("kd", d.kd, "kh", d.kh, "kw", d.kw);

    if (canonical || d.sh != 1 || d.sw != 1 || d.sd != 1)
        print_spatial("sd", d.sd, "sh", d.sh, "sw", d.sw);

    print_spatial("pd", d.pd, "ph", d.ph, "pw", d.pw);

    if (canonical || d.dh != 0 || d.dw != 0 || d.dd != 0)
        print_spatial("dd", d.dd, "dh", d.dh, "dw", d.dw);

    if (!d.name.empty()) s << "n" << d.name;

    return s;
}

dims_t desc_t::src_dims() const {
    dims_t src_dims {mb, ic, id, ih, iw};
    for (int d = 0; d < 5 - ndims; ++d) {
        src_dims.erase(src_dims.begin() + 2);
    }

    return src_dims;
}

dims_t desc_t::wei_dims() const {
    dims_t wei_dims {g, oc / g, ic / g, kd, kh, kw};
    if (!has_groups) { wei_dims.erase(wei_dims.begin()); }
    for (int d = 0; d < 5 - ndims; ++d) {
        wei_dims.erase(wei_dims.begin() + 2 + has_groups);
    }

    return wei_dims;
}

dims_t desc_t::bia_dims() const {
    dims_t bia_dims {oc};
    return bia_dims;
}

dims_t desc_t::dst_dims() const {
    dims_t dst_dims {mb, oc, od, oh, ow};
    for (int d = 0; d < 5 - ndims; ++d) {
        dst_dims.erase(dst_dims.begin() + 2);
    }

    return dst_dims;
}

dims_t desc_t::strides() const {
    dims_t strides {sd, sh, sw};
    return dims_t(strides.begin() + (5 - ndims), strides.end());
}

dims_t desc_t::dilations() const {
    dims_t dilations {dd, dh, dw};
    return dims_t(dilations.begin() + (5 - ndims), dilations.end());
}

dims_t desc_t::padding() const {
    dims_t padding {pd, ph, pw};
    return dims_t(padding.begin() + (5 - ndims), padding.end());
}

dims_t desc_t::padding_r() const {
    dims_t padding_r {pd_r, ph_r, pw_r};
    return dims_t(padding_r.begin() + (5 - ndims), padding_r.end());
}

int64_t desc_t::desc_nelems(int arg, int mask) const {
    std::vector<int64_t> src {mb, ic, id, ih, iw};
    std::vector<int64_t> wei {g, oc, ic, kd, kh, kw};
    std::vector<int64_t> dst {mb, oc, od, oh, ow};
    std::vector<int64_t> dummy;

    if (!has_groups) wei.erase(wei.begin());

    for (int d = 0; d < 5 - ndims; d++) {
        src.erase(src.begin() + 2);
        wei.erase(wei.begin() + 2);
        dst.erase(dst.begin() + 2);
    }

    std::vector<int64_t> &dims = dummy;
    switch (arg) {
        case DNNL_ARG_SRC: dims = src; break;
        case DNNL_ARG_WEIGHTS: dims = wei; break;
        case DNNL_ARG_DST: dims = dst; break;
        default: assert(!"unsupported arg");
    }

    int64_t nelems = 1;
    for (int d = 0; d < ndims; d++) {
        nelems *= (mask & (1 << d)) ? dims[d] : 1;
    }
    return nelems;
}

void prb_t::count_ops() {
    if (ops > 0) return;

    int64_t od_t = this->id;
    int64_t oh_t = this->ih;
    int64_t ow_t = this->iw;
    int64_t id_t = this->od;
    int64_t ih_t = this->oh;
    int64_t iw_t = this->ow;
    double sp_ops = 0;
    for_(int64_t od = 0; od < od_t; ++od)
    for_(int64_t oh = 0; oh < oh_t; ++oh)
    for (int64_t ow = 0; ow < ow_t; ++ow) {
        for (int64_t kd = 0; kd < this->kd; ++kd) {
            const int64_t id = od * this->sd - this->pd + kd * (this->dd + 1);
            if (id < 0 || id >= id_t) continue;
            for (int64_t kh = 0; kh < this->kh; ++kh) {
                const int64_t ih
                        = oh * this->sh - this->ph + kh * (this->dh + 1);
                if (ih < 0 || ih >= ih_t) continue;
                for (int64_t kw = 0; kw < this->kw; ++kw) {
                    const int64_t iw
                            = ow * this->sw - this->pw + kw * (this->dw + 1);
                    if (iw < 0 || iw >= iw_t) continue;
                    sp_ops += 1;
                }
            }
        }
    }

    ops = 2 * this->mb * this->oc * this->ic / this->g * sp_ops;
}

float *prb_t::generate_scales(int arg) {
    const auto &scales = attr.scales;
    if (scales.is_def()) return nullptr;

    const auto &e = scales.get(arg);
    if (e.policy == policy_t::COMMON) {
        float *s = (float *)zmalloc(sizeof(float), 4);
        SAFE_V(s != nullptr ? OK : FAIL);
        s[0] = e.scale;
        return s;
    }

    assert(e.policy == policy_t::PER_OC);
    auto mask = attr_t::get_default_mask(e.policy, arg);
    if (arg == DNNL_ARG_WEIGHTS && has_groups) mask = (1 << mask) + 1;
    int64_t s_nelems = desc_nelems(arg, mask);

    float *s = (float *)zmalloc(sizeof(float) * s_nelems, 64);
    SAFE_V(s != nullptr ? OK : FAIL);

    const float K = 32;
    /* scale in [1/K .. K], with starting point at e.scale */
    float s_val[2] = {e.scale, e.scale / 2};
    for (int64_t i = 0; i < s_nelems; ++i) {
        int64_t si = i % 2; // 0 -> left, 1 -> right
        s[i] = s_val[si];
        if (si == 0) {
            s_val[si] /= 2.;
            // turn around to become ~K
            if (s_val[si] < 1. / K) s_val[si] *= K * K;
        } else {
            s_val[si] *= 2.;
            // turn around to become ~K
            if (s_val[si] > K) s_val[si] /= K * K;
        }
    }
    return s;
}

int32_t *prb_t::generate_zero_points(int arg) {
    const auto &zp = attr.zero_points;
    if (zp.is_def(arg)) return nullptr;

    const auto &e = zp.get(arg);
    const auto mask = attr_t::get_default_mask(e.policy);
    int64_t zp_nelems = desc_nelems(arg, mask);

    int32_t *ptr = (int32_t *)zmalloc(sizeof(int32_t) * zp_nelems, 64);
    SAFE_V(ptr != nullptr ? OK : FAIL);

    for (int i = 0; i < zp_nelems; ++i)
        ptr[i] = e.value + i % 3;

    return ptr;
}

std::ostream &operator<<(std::ostream &s, const prb_t &prb) {
    dump_global_params(s);
    settings_t def;

    if (canonical || prb.dir != def.dir[0]) s << "--dir=" << prb.dir << " ";
    if (canonical || prb.cfg != def.cfg[0]) s << "--cfg=" << prb.cfg << " ";
    if (canonical || prb.stag != def.stag[0]) s << "--stag=" << prb.stag << " ";
    if (canonical || prb.wtag != def.wtag[0]) s << "--wtag=" << prb.wtag << " ";
    if (canonical || prb.dtag != def.dtag[0]) s << "--dtag=" << prb.dtag << " ";
    if (canonical || prb.alg != def.alg[0])
        s << "--alg=" << alg2str(prb.alg) << " ";

    s << prb.attr;
    if (canonical || prb.ctx_init != def.ctx_init[0])
        s << "--ctx-init=" << prb.ctx_init << " ";
    if (canonical || prb.ctx_exe != def.ctx_exe[0])
        s << "--ctx-exe=" << prb.ctx_exe << " ";

    s << static_cast<const desc_t &>(prb);

    return s;
}

} // namespace deconv
