################################################################################
# Copyright 2021-2022 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################
# Initial version generated by Sphinx quickstart from template
# https://github.com/sphinx-doc/sphinx/blob/4.x/sphinx/templates/quickstart/conf.py_t
################################################################################

# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
import subprocess

def whereis(binary):
    command = 'which' if os.name != 'nt' else 'where'
    try:
        sub = subprocess.run([command, binary], text=True, capture_output=True)
        return sub.stdout
    except BaseException as e:
        print(f"Warning: {binary} is not found")
        return ''

doxyrest_bin_path = whereis('doxyrest')
doxyrest_dir_path = os.path.dirname(doxyrest_bin_path)
doxyrest_share_path = doxyrest_dir_path + "/../share/doxyrest/sphinx"
sys.path.insert(1, os.path.abspath(doxyrest_share_path))


# -- Project information -----------------------------------------------------

project = 'oneDNN'
copyright = '2016-2022 Intel Corporation'
author = ''

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.

# Specify the path to Doxyrest extensions for Sphinx:

# Add Doxyrest extensions ``doxyrest`` and ``cpplexer``:

extensions = ['doxyrest', 'cpplexer']

# If you used INTRO_FILE in 'doxyrest-config.lua' to force-include it
# into 'index.rst', exclude it from the Sphinx input (otherwise, there
# will be build warnings):


imgmath_latex_preamble = '''\\usepackage[T1]{fontenc}
\\usepackage[utf8]{inputenc}
\\usepackage{textgreek}
\\newcommand{\\src}{\\operatorname{src}}
\\newcommand{\\srclayer}{\\operatorname{src\\_layer}}
\\newcommand{\\srclayerattention}{\\operatorname{src\\_layer\\_attention}}
\\newcommand{\\srciter}{\\operatorname{src\\_iter}}
\\newcommand{\\srciterc}{\\operatorname{src\\_iter\\_c}}
\\newcommand{\\weights}{\\operatorname{weights}}
\\newcommand{\\weightslayer}{\\operatorname{weights\\_layer}}
\\newcommand{\\weightsiter}{\\operatorname{weights\\_iter}}
\\newcommand{\\weightspeephole}{\\operatorname{weights\\_peephole}}
\\newcommand{\\weightsprojection}{\\operatorname{weights\\_projection}}
\\newcommand{\\bias}{\\operatorname{bias}}
\\newcommand{\\dst}{\\operatorname{dst}}
\\newcommand{\\dstlayer}{\\operatorname{dst\\_layer}}
\\newcommand{\\dstiter}{\\operatorname{dst\\_iter}}
\\newcommand{\\dstiterc}{\\operatorname{dst\\_iter\\_c}}
\\newcommand{\\diffsrc}{\\operatorname{diff\\_src}}
\\newcommand{\\diffsrclayer}{\\operatorname{diff\\_src\\_layer}}
\\newcommand{\\diffsrclayerattention}{\\operatorname{diff\\_src\\_layer\\_attention}}
\\newcommand{\\diffsrciter}{\\operatorname{diff\\_src\\_iter}}
\\newcommand{\\diffsrciterc}{\\operatorname{diff\\_src\\_iter\\_c}}
\\newcommand{\\diffweights}{\\operatorname{diff\\_weights}}
\\newcommand{\\diffweightslayer}{\\operatorname{diff\\_weights\\_layer}}
\\newcommand{\\diffweightsiter}{\\operatorname{diff\\_weights\\_iter}}
\\newcommand{\\diffweightspeephole}{\\operatorname{diff\\_weights\\_peephole}}
\\newcommand{\\diffweightsprojection}{\\operatorname{diff\\_weights\\_projection}}
\\newcommand{\\diffbias}{\\operatorname{diff\\_bias}}
\\newcommand{\\diffdst}{\\operatorname{diff\\_dst}}
\\newcommand{\\diffdstlayer}{\\operatorname{diff\\_dst\\_layer}}
\\newcommand{\\diffdstiter}{\\operatorname{diff\\_dst\\_iter}}
\\newcommand{\\diffdstiterc}{\\operatorname{diff\\_dst\\_iter\\_c}}
\\newcommand{\\diffgamma}{\\operatorname{diff\\_\\gamma}}
\\newcommand{\\diffbeta}{\\operatorname{diff\\_\\beta}}
\\newcommand{\\workspace}{\\operatorname{workspace}}'''



# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store',]

exclude_patterns += ['page_index.rst']
# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_book_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']
#html_js_files = [('dnnl.js', {'defer': 'defer'})]


mathjax3_config = {
'tex': {
    'macros': {
        'src': '\\operatorname{src}',
        'srclayer': '\\operatorname{src\\_layer}',
        'srclayerattention': '\\operatorname{src\\_layer\\_attention}',
        'srciter': '\\operatorname{src\\_iter}',
        'srciterc': '\\operatorname{src\\_iter\\_c}',
        'weights': '\\operatorname{weights}',
        'weightslayer': '\\operatorname{weights\\_layer}',
        'weightsiter': '\\operatorname{weights\\_iter}',
        'weightspeephole': '\\operatorname{weights\\_peephole}',
        'weightsprojection': '\\operatorname{weights\\_projection}',
        'bias': '\\operatorname{bias}',
        'dst': '\\operatorname{dst}',
        'dstlayer': '\\operatorname{dst\\_layer}',
        'dstiter': '\\operatorname{dst\\_iter}',
        'dstiterc': '\\operatorname{dst\\_iter\\_c}',
        'diffsrc': '\\operatorname{diff\\_src}',
        'diffsrclayer': '\\operatorname{diff\\_src\\_layer}',
        'diffsrclayerattention': '\\operatorname{diff\\_src\\_layer\\_attention}',
        'diffsrciter': '\\operatorname{diff\\_src\\_iter}',
        'diffsrciterc': '\\operatorname{diff\\_src\\_iter\\_c}',
        'diffweights': '\\operatorname{diff\\_weights}',
        'diffweightslayer': '\\operatorname{diff\\_weights\\_layer}',
        'diffweightsiter': '\\operatorname{diff\\_weights\\_iter}',
        'diffweightspeephole': '\\operatorname{diff\\_weights\\_peephole}',
        'diffweightsprojection': '\\operatorname{diff\\_weights\\_projection}',
        'diffbias': '\\operatorname{diff\\_bias}',
        'diffdst': '\\operatorname{diff\\_dst}',
        'diffdstlayer': '\\operatorname{diff\\_dst\\_layer}',
        'diffdstiter': '\\operatorname{diff\\_dst\\_iter}',
        'diffdstiterc': '\\operatorname{diff\\_dst\\_iter\\_c}',
        'diffgamma': '\\operatorname{diff\\_\\gamma}',
        'diffbeta': '\\operatorname{diff\\_\\beta}',
        'workspace': '\\operatorname{workspace}',
        'srcshape': '\\operatorname{src\\_\\shape}',
        'dstshape': '\\operatorname{dst\\_\\shape}'
        }
    }
}

def setup(app):
    app.connect('env-before-read-docs', fixFileNameRefs)
    app.connect('env-before-read-docs', addTocTrees)

def fixFileNameRefs(app, env, docnames):

    replacements = {"page_dev_guide": "dev_guide", "group_Dnnl":"group_dnnl"}
    targetDir = "rst"

    fileExtension = ".rst"


    for dirpath, dirnames, filenames in os.walk(targetDir):
        for filename in [f for f in filenames if f.endswith(fileExtension)]:
            filePath = os.path.join(dirpath,filename)
            print("replacing strings in " + filePath)
            outdata = None
            with open(filePath) as f:
                read_data = f.read()
                outdata = read_data
                for replacement in replacements:
                    outdata = outdata.replace(replacement,replacements[replacement])
            with open(filePath,"w") as f:
                f.write(outdata)



def addTocTrees(app, env, docnames):

    trees2Add = {'rst/dev_guide_inference_and_training_aspects.rst':['dev_guide_inference.rst','dev_guide_inference_int8.rst','dev_guide_training_bf16.rst'],
                 'rst/dev_guide_attributes.rst':['dev_guide_attributes_fpmath_mode.rst','dev_guide_attributes_quantization.rst','dev_guide_attributes_post_ops.rst','dev_guide_attributes_scratchpad.rst'],
                 'rst/graph_supported_operations.rst':[
                    'dev_guide_op_abs.rst',
                    'dev_guide_op_absbackward.rst',
                    'dev_guide_op_add.rst',
                    'dev_guide_op_avgpool.rst',
                    'dev_guide_op_avgpoolbackward.rst',
                    'dev_guide_op_batchnormforwardtraining.rst',
                    'dev_guide_op_batchnorminference.rst',
                    'dev_guide_op_batchnormtrainingbackward.rst',
                    'dev_guide_op_biasadd.rst',
                    'dev_guide_op_biasaddbackward.rst',
                    'dev_guide_op_clamp',
                    'dev_guide_op_clampbackward',
                    'dev_guide_op_concat.rst',
                    'dev_guide_op_convolution.rst',
                    'dev_guide_op_convolutionbackwarddata.rst',
                    'dev_guide_op_convolutionbackwardweights.rst',
                    'dev_guide_op_convtranspose.rst',
                    'dev_guide_op_convtransposebackwarddata.rst',
                    'dev_guide_op_convtransposebackwardweights.rst',
                    'dev_guide_op_dequantize.rst',
                    'dev_guide_op_divide.rst',
                    'dev_guide_op_dynamicdequantize.rst',
                    'dev_guide_op_dynamicquantize.rst',
                    'dev_guide_op_elu',
                    'dev_guide_op_elubackward',
                    'dev_guide_op_end',
                    'dev_guide_op_exp',
                    'dev_guide_op_gelu',
                    'dev_guide_op_gelubackward',
                    'dev_guide_op_hardswish',
                    'dev_guide_op_hardswishbackward',
                    'dev_guide_op_interpolate.rst',
                    'dev_guide_op_interpolatebackward.rst',
                    'dev_guide_op_layernorm.rst',
                    'dev_guide_op_layernormbackward.rst',
                    'dev_guide_op_leakyrelu',
                    'dev_guide_op_log',
                    'dev_guide_op_logsoftmax',
                    'dev_guide_op_logsoftmaxbackward',
                    'dev_guide_op_matmul.rst',
                    'dev_guide_op_maximum.rst',
                    'dev_guide_op_maxpool.rst',
                    'dev_guide_op_maxpoolbackward.rst',
                    'dev_guide_op_minimum.rst',
                    'dev_guide_op_mish',
                    'dev_guide_op_mishbackward',
                    'dev_guide_op_multiply.rst',
                    'dev_guide_op_prelu',
                    'dev_guide_op_prelubackward',
                    'dev_guide_op_quantize.rst',
                    'dev_guide_op_reciprocal.rst',
                    'dev_guide_op_reducel1.rst',
                    'dev_guide_op_reducel2.rst',
                    'dev_guide_op_reducemax.rst',
                    'dev_guide_op_reducemean.rst',
                    'dev_guide_op_reducemin.rst',
                    'dev_guide_op_reduceprod.rst',
                    'dev_guide_op_reducesum.rst',
                    'dev_guide_op_relu.rst',
                    'dev_guide_op_relubackward',
                    'dev_guide_op_reorder.rst',
                    'dev_guide_op_round',
                    'dev_guide_op_sigmoid',
                    'dev_guide_op_sigmoidbackward',
                    'dev_guide_op_softmax',
                    'dev_guide_op_softmaxbackward',
                    'dev_guide_op_softplus',
                    'dev_guide_op_softplusbackward',
                    'dev_guide_op_sqrt',
                    'dev_guide_op_sqrtbackward',
                    'dev_guide_op_square',
                    'dev_guide_op_squareddifference',
                    'dev_guide_op_staticreshape',
                    'dev_guide_op_statictranspose',
                    'dev_guide_op_subtract.rst',
                    'dev_guide_op_tanh',
                    'dev_guide_op_tanhbackward',
                    'dev_guide_op_typecast.rst',
                    'dev_guide_op_wildcard'
                    ],
                 'rst/graph_programming_model.rst':['dev_guide_graph_basic_concepts.rst', 'dev_guide_graph_low_precision.rst']}


    for rstFile in trees2Add:
        with open(rstFile, 'r') as f:
            fContents = f.read()
            if '.. toctree::' in fContents:
                continue
        with open(rstFile, 'a+') as f:
            tocTree = '\n.. toctree::\n   :hidden:\n\n'
            for file in trees2Add[rstFile]:
                tocTree += "   " + file + "\n"
            tocTree += "\n\n"
            f.write(tocTree)
