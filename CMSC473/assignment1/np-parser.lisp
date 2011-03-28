;;;;
;;;; CMSC 473 Assignment 1
;;;; James Lee <jlee23@umbc.edu>
;;;;
;;;; A grammar and parser for English noun phrases
;;;;


;; load SPLIT-SEQUENCE package from
;; http://www.cliki.net/SPLIT-SEQUENCE
;; seemeingly released to the public domain
(load "split-sequence/split-sequence.lisp")


;;;;
;;;; English noun phrase grammar
;;;;


;; NP        -> NP3Sg | NPNon3Sg | PPron
;; NP3Sg     -> DetSg (Card) (Ord) (Quant) (AdjP) NominalSg
;; NPNon3Sg  -> (DetPl) (Card) (Ord) (Quant) (AdjP) NominalPl
;; DetSg     -> Det | Dem-DetSg | Wh-Det | PoPron
;; DetPl     -> Det | Dem-DetPl | Wh-Det | PoPron
;; AdjP      -> Adj | Adv Adj
;; NominalSg -> NSg | N NSg | NominalSg PP | NominalSg Rel-ClSg
;; NominalPl -> NPl | N NPl | NominalPl PP | NominalPl Rel-ClPl
;; N         -> NSg | NPl
;; PP        -> Prep NP
;; Rel-ClSg  -> Rel-Pro V3Sg NP | Rel-Pro VPastPP NP
;; Rel-ClPl  -> Rel-Pro V NP | Rel-Pro VPastPP NP


;;; Function: generate-np3sg
;;; Arguments:
;;;   none
;;; Returns:
;;;   a list of rules containing all combinations of optional symbols for
;;;   singular noun phrases
(defun generate-np3sg ()
  (loop for i from 0 to 15 collect
	(let ((rules '(nominalsg)))
	  (if (= (logand i 1) 1) (push 'adjp rules))
	  (if (= (logand i 2) 2) (push 'quant rules))
	  (if (= (logand i 4) 4) (push 'ord rules))
	  (if (= (logand i 8) 8) (push 'card rules))
	  (push 'detsg rules)
	  rules)))

	
;;; Function: generate-np3sg
;;; Arguments:
;;;   none
;;; Returns:
;;;   a list of rules containing all combinations of optional symbols for
;;;   plural noun phrases
(defun generate-npnon3sg ()
  (loop for i from 0 to 31 collect
	(let ((rules '(nominalpl)))
	  (if (= (logand i 1) 1) (push 'adjp rules))
	  (if (= (logand i 2) 2) (push 'quant rules))
	  (if (= (logand i 4) 4) (push 'ord rules))
	  (if (= (logand i 8) 8) (push 'card rules))
	  (if (= (logand i 16) 16) (push 'detpl rules))
	  rules)))



;; global grammar varibles
(defparameter *grammar* (make-hash-table))
(defparameter *parts-of-speech* '(det prep nsg npl quant adj v v3sg vpastpp card ord ppron popron rel-pro dem-detsg dem-detpl adv wh-det))

;; grammatical rules
(setf (gethash 'np *grammar*) '((np3sg) (npnon3sg) (ppron)))
(setf (gethash 'np3sg *grammar*) (generate-np3sg))
(setf (gethash 'npnon3sg *grammar*) (generate-npnon3sg))
(setf (gethash 'detsg *grammar*) '((det) (dem-detsg) (wh-det) (popron)))
(setf (gethash 'detpl *grammar*) '((det) (dem-detpl) (wh-det) (popron)))
(setf (gethash 'adjp *grammar*) '((adj) (adv adj)))
(setf (gethash 'nominalsg *grammar*) '((nsg) (n nsg) (nominalsg pp) (nominalsg rel-clsg)))
(setf (gethash 'nominalpl *grammar*) '((npl) (n npl) (nominalpl pp) (nominalpl rel-clpl)))
(setf (gethash 'n *grammar*) '((nsg) (npl)))
(setf (gethash 'pp *grammar*) '((prep np)))
(setf (gethash 'rel-clsg *grammar*) '((rel-pro v3sg np) (rel-pro vpastpp np)))
(setf (gethash 'rel-clpl *grammar*) '((rel-pro v np) (rel-pro vpastpp np)))

;; lexical rules
(setf (gethash 'det *grammar*) '("a" "an" "the"))
(setf (gethash 'prep *grammar*) '("at" "in" "of" "on"))
(setf (gethash 'nsg *grammar*) '("airplane" "airport" "restaurant" "runway"))
(setf (gethash 'npl *grammar*) '("airplanes" "airplanes" "restaurants" "runways" "houses"))
(setf (gethash 'quant *grammar*) '("any" "many"))
(setf (gethash 'adj *grammar*) '("beautiful" "big" "new"))
(setf (gethash 'v *grammar*) '("eat"))
(setf (gethash 'v3sg *grammar*) '("eats"))
(setf (gethash 'vpastpp *grammar*) '("finished" "offered" "built"))
(setf (gethash 'card *grammar*) '("four" "three" "two"))
(setf (gethash 'ord *grammar*) '("fourth" "second" "third"))
(setf (gethash 'ppron *grammar*) '("he" "you"))
(setf (gethash 'popron *grammar*) '("his" "my"))
(setf (gethash 'rel-pro *grammar*) '("that"))
(setf (gethash 'dem-detsg *grammar*) '("that" "this"))
(setf (gethash 'dem-detpl *grammar*) '("these" "those"))
(setf (gethash 'adv *grammar*) '("very"))
(setf (gethash 'wh-det *grammar*) '("which" "who"))



;;;;
;;;; Parsing functions
;;;;



;;; Structure: state
;;; Represents one state in the dynamic programming table for the Earley
;;; algorithm.
(defstruct state
  production		; the current rule from the grammar that is being processed
  (subtree-position 0)	; the position in the phrase where the production begins
  (dot-position 0)	; the position in the cdr of the production before the next symbol
  children)		; child states of the production added by the completer



;;; Function: parse
;;; Arguments:
;;;   str - a noun phrase as a string to parse
;;; Returns:
;;;   a parse tree if the string is a noun phrase or nil
(defun parse (str)
  (if (stringp str)
    (parse-tree-from-earley-chart
      (earley-parse
	(split-sequence:SPLIT-SEQUENCE #\Space str :remove-empty-subseqs t)))))



;;; Function: parse-tree-from-earley-chart
;;; Arguments:
;;;   chart - an array as returned by earley-parse
;;; Returns:
;;;   a parse tree if the chart represents a completed parse
(defun parse-tree-from-earley-chart (chart)
  (let ((last-state (car (last (aref chart (- (car (array-dimensions chart)) 1))))))
    (if (and last-state (equal (state-production last-state) '(gamma np)))
      (second (parse-tree-from-earley-state last-state)))))



;;; Function: parse-tree-from-earley-state
;;; Arguments:
;;;   state - a single state from the earley-parse chart
;;; Returns:
;;;   a parse tree representing the state and its children
(defun parse-tree-from-earley-state (state)
  (if (state-children state)
    (append (list (car (state-production state))) (mapcar #'parse-tree-from-earley-state (state-children state)))
    (list (car (state-production state)) (cdr (state-production state)))))



;;; Function: incomplete?
;;; Arguments:
;;;   state - a single state from the earley-parse chart
;;; Returns:
;;;   t if the dot is before the end of the state's production or nil
(defun incomplete? (state)
  (< (state-dot-position state) (list-length (cdr (state-production state)))))



;;; Function: next-cat
;;; Arguments:
;;;   state - a single state from the earley-parse chart
;;; Returns:
;;;   the symbol after the dot in the state's production
(defun next-cat (state)
  (nth (state-dot-position state) (cdr (state-production state))))



;;; Function: earley-parse
;;; Arguments:
;;;   words - a list of strings which make up a noun phrase
;;; Returns:
;;;   A dynamic programming chart of the parsing process
;;;
;;; This function implements the algorithm shown in section 10.4 of
;;; "Speech and Language Processing"
(defun earley-parse (words &aux (chart (make-array (+ (list-length words) 1))))
  (enqueue (make-state :production '(gamma np)
		       :subtree-position 0
		       :dot-position 0)
	   (aref chart 0))
  (dotimes (i (+ (list-length words) 1))
    (do ((j 0 (+ j 1)))
      ((>= j (list-length (aref chart i))))
      (let* ((state (nth j (aref chart i)))
	     (next (next-cat state)))
	(cond ((and (incomplete? state) (not (find next *parts-of-speech*)))
	       ;; predictor
	       (dolist (rule (gethash next *grammar*))
		 (enqueue (make-state :production (append (list next) rule)
				      :subtree-position i
				      :dot-position 0)
			  (aref chart i))))
	      ((and (incomplete? state) (find next *parts-of-speech*))
	       ;; scanner
	       (let ((word (nth i words)))
		 (if (find word (gethash next *grammar*) :test #'(lambda (x y)
								   (string= (string-upcase x)
									    (string-upcase y))))
		   (enqueue (make-state :production (list next word)
					:subtree-position i
					:dot-position 1)
			    (aref chart (+ i 1))))))
	      (t
		;; completer
		(dolist (old-state (remove-if #'(lambda (x)
						  (not (eql (next-cat x)
							    (car (state-production state)))))
					      (aref chart (state-subtree-position state))))
		  (enqueue (make-state :production (state-production old-state)
				       :subtree-position (state-subtree-position old-state)
				       :dot-position (+ (state-dot-position old-state) 1)
				       :children (append (state-children old-state) (list state)))
			   (aref chart i))))))))
  chart)



;;; Macro: enqueue
;;; Arguments:
;;;   state - the state to append to the chart-entry
;;;   chart-entry - the chart to append the state into destructively
;;;
;;; The state is only appended if it doesn't yet exist in the chart-entry
(defmacro enqueue (state chart-entry)
  `(unless (find ,state ,chart-entry :test #'(lambda (x y)
					       (and (equal (state-production x)
							   (state-production y))
						    (equal (state-subtree-position x)
							   (state-subtree-position y))
						    (equal (state-dot-position x)
							   (state-dot-position y)))))
     (setf ,chart-entry (append ,chart-entry (list ,state)))))
