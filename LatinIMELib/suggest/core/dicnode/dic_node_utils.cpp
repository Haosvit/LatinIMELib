/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dic_node_utils.h"

#include "dic_node.h"
#include "dic_node_vector.h"
#include "multi_bigram_map.h"
#include "dictionary_structure_with_buffer_policy.h"

namespace latinime {

///////////////////////////////
// Node initialization utils //
///////////////////////////////

/* static */ void DicNodeUtils::initAsRoot(
        const DictionaryStructureWithBufferPolicy *const dictionaryStructurePolicy,
        const int *const prevWordsPtNodePos, DicNode *const newRootDicNode) {
    newRootDicNode->initAsRoot(dictionaryStructurePolicy->getRootPosition(), prevWordsPtNodePos);
}

/*static */ void DicNodeUtils::initAsRootWithPreviousWord(
        const DictionaryStructureWithBufferPolicy *const dictionaryStructurePolicy,
        const DicNode *const prevWordLastDicNode, DicNode *const newRootDicNode) {
    newRootDicNode->initAsRootWithPreviousWord(
            prevWordLastDicNode, dictionaryStructurePolicy->getRootPosition());
}

/* static */ void DicNodeUtils::initByCopy(const DicNode *const srcDicNode,
        DicNode *const destDicNode) {
    destDicNode->initByCopy(srcDicNode);
}

///////////////////////////////////
// Traverse node expansion utils //
///////////////////////////////////
/* static */ void DicNodeUtils::getAllChildDicNodes(const DicNode *dicNode,
        const DictionaryStructureWithBufferPolicy *const dictionaryStructurePolicy,
        DicNodeVector *const childDicNodes) {
    if (dicNode->isTotalInputSizeExceedingLimit()) {
        return;
    }
    if (!dicNode->isLeavingNode()) {
        childDicNodes->pushPassingChild(dicNode);
    } else {
        dictionaryStructurePolicy->createAndGetAllChildDicNodes(dicNode, childDicNodes);
    }
}

///////////////////
// Scoring utils //
///////////////////
/**
 * Computes the combined bigram / unigram cost for the given dicNode.
 */
/* static */ float DicNodeUtils::getBigramNodeImprobability(
        const DictionaryStructureWithBufferPolicy *const dictionaryStructurePolicy,
        const DicNode *const dicNode, MultiBigramMap *const multiBigramMap) {
    if (dicNode->hasMultipleWords() && !dicNode->isValidMultipleWordSuggestion()) {
        return static_cast<float>(MAX_VALUE_FOR_WEIGHTING);
    }
    const int probability = getBigramNodeProbability(dictionaryStructurePolicy, dicNode,
            multiBigramMap);
    // TODO: This equation to calculate the improbability looks unreasonable.  Investigate this.
    const float cost = static_cast<float>(MAX_PROBABILITY - probability)
            / static_cast<float>(MAX_PROBABILITY);
    return cost;
}

/* static */ int DicNodeUtils::getBigramNodeProbability(
        const DictionaryStructureWithBufferPolicy *const dictionaryStructurePolicy,
        const DicNode *const dicNode, MultiBigramMap *const multiBigramMap) {
    const int unigramProbability = dicNode->getProbability();
    if (multiBigramMap) {
        const int *const prevWordsPtNodePos = dicNode->getPrevWordsTerminalPtNodePos();
        return multiBigramMap->getBigramProbability(dictionaryStructurePolicy,
                prevWordsPtNodePos, dicNode->getPtNodePos(), unigramProbability);
    }
    return dictionaryStructurePolicy->getProbability(unigramProbability,
            NOT_A_PROBABILITY);
}

} // namespace latinime
