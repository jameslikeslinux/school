;;;;
;;;; CMSC 473 Assignment 2
;;;; James Lee <jlee23@umbc.edu>
;;;;
;;;; A grammar and parser for English noun phrases
;;;; that uses feature structures and unification to 
;;;; parse noun phrases with agreement.
;;;;


;; load SPLIT-SEQUENCE package from
;; http://www.cliki.net/SPLIT-SEQUENCE
;; seemeingly released to the public domain
(load "split-sequence/split-sequence.lisp")


;;;;
;;;; English noun phrase grammar
;;;;


(defparameter *grammar* (make-hash-table))
(defparameter *parts-of-speech* '(det prep noun quant adj verb vpastpp card ord ppron rel-pro adv))

(setf empty '((content nil) (pointer nil)))
(setf agreement `((content ((agreement ,empty))) (pointer nil)))

(defun generate-np ()
  (loop for i from 0 to 31 collect
	(let ((rules '(nominal))
	      (dag `((nominal ,agreement))))
	  (if (= (logand i 1) 1) (progn (push 'adjp rules) (push `(adjp ,empty) dag)))
	  (if (= (logand i 2) 2) (progn (push 'quant rules) (push `(quant ,empty) dag)))
	  (if (= (logand i 4) 4) (progn (push 'ord rules) (push `(ord ,empty) dag)))
	  (if (= (logand i 8) 8) (progn (push 'card rules) (push `(card ,empty) dag)))
	  (if (= (logand i 16) 16) (progn (push 'det rules) (push `(det ((content nil) (pointer ,agreement))) dag)))
	  (push `(np ((content nil) (pointer ,agreement))) dag)
	  `(,rules ((content ,dag) (pointer nil))))))


;; NP -> (Det) (Card) (Ord) (Quant) (AdjP) Nominal
;; 	<NP agreement> = <Nominal agreement>
;; 	<Det agreement> = <Nominal agreement>
;; NP -> PPron
(setf (gethash 'np *grammar*) (append (generate-np) `(((ppron) ((content ((np ,empty) (ppron ,empty))) (pointer nil))))))

(defmacro add-to-grammar (rule production dag)
  `(setf (gethash ,rule *grammar*) (append (gethash ,rule *grammar*) (list (list ,production ,dag)))))


;; AdjP -> Adj | Adv Adj
(add-to-grammar 'adjp '(adj) `((content ((adjp ,empty) (adj ,empty))) (pointer nil)))
(add-to-grammar 'adjp '(adv adj) `((content ((adjp ,empty) (adv ,empty) (adj ,empty))) (pointer nil)))

;; Nominal -> Noun
;; 	<Nominal agreement> = <Noun agreement>
;; Nominal -> Noun Noun
;; 	<Nominal agreement> = <Noun agreement>
;; Nominal -> Nominal PP
;; 	<Nominal agreement> = <Nominal agreement>
;; Nominal -> Nominal Rel-Cl
;; 	<Nominal agreement> = <Nominal agreement>
;; 	<Nominal agreement> = <Rel-Cl agreement>
(add-to-grammar 'nominal '(noun) `((content ((nominal ((content nil) (pointer ,agreement))) (noun ,agreement))) (pointer nil)))
;(add-to-grammar 'nominal '(noun noun) `((content ((nominal ((content nil) (pointer ,agreement))) (noun ,empty) (noun ,agreement))) (pointer nil)))
(add-to-grammar 'nominal '(nominal pp) `((content ((nominal ((content nil) (pointer ,agreement))) (nominal ,agreement) (pp ,empty))) (pointer nil)))
(add-to-grammar 'nominal '(nominal rel-cl) `((content ((nominal ((content nil) (pointer ,agreement))) (nominal ((content nil) (pointer ,agreement))) (rel-cl ,agreement))) (pointer nil)))

;; PP -> Prep NP
(add-to-grammar 'pp '(prep np) `((content ((pp ,empty) (prep ,empty) (np ,empty))) (pointer nil)))

;; Rel-Cl -> Rel-Pro Verb NP
;; 	<Rel-Cl agreement> = <Verb agreement>
;; Rel-Cl -> Rel-Pro VPastPP NP
(add-to-grammar 'rel-cl '(rel-pro verb np) `((content ((rel-cl ((content nil) (pointer ,agreement))) (rel-pro ,empty) (verb ,agreement) (np ,empty))) (pointer nil)))
(add-to-grammar 'rel-cl '(rel-pro vpastpp np) `((content ((rel-cl ,empty) (vpastpp ,empty) (np ,empty))) (pointer nil)))


;; lexical rules
(add-to-grammar 'det "a" `((content ((det ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "an" `((content ((det ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "the" `((content ((det ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "that" `((content ((det ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "this" `((content ((det ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "these" `((content ((det ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "those" `((content ((det ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'det "his" `((content ((det ,empty))) (pointer nil)))
(add-to-grammar 'det "my" `((content ((det ,empty))) (pointer nil)))
(add-to-grammar 'det "which" `((content ((det ,empty))) (pointer nil)))
(add-to-grammar 'det "who" `((content ((det ,empty))) (pointer nil)))

(add-to-grammar 'prep "at" `((content ((prep ,empty))) (pointer nil)))
(add-to-grammar 'prep "in" `((content ((prep ,empty))) (pointer nil)))
(add-to-grammar 'prep "of" `((content ((prep ,empty))) (pointer nil)))
(add-to-grammar 'prep "on" `((content ((prep ,empty))) (pointer nil)))

(add-to-grammar 'noun "airplane" `((content ((noun ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "airplanes" `((content ((noun ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "airport" `((content ((noun ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "airports" `((content ((noun ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "restaurant" `((content ((noun ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "restaurants" `((content ((noun ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "runway" `((content ((noun ((content ((agreement ((content ((number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "runways" `((content ((noun ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'noun "houses" `((content ((noun ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))

(add-to-grammar 'quant "any" `((content ((quant ,empty))) (pointer nil)))
(add-to-grammar 'quant "many" `((content ((quant ,empty))) (pointer nil)))

(add-to-grammar 'adj "beautiful" `((content ((adj ,empty))) (pointer nil)))
(add-to-grammar 'adj "big" `((content ((adj ,empty))) (pointer nil)))
(add-to-grammar 'adj "new" `((content ((adj ,empty))) (pointer nil)))

(add-to-grammar 'verb "eat" `((content ((verb ((content ((agreement ((content ((person ((content 1) (pointer nil))) (number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'verb "eat" `((content ((verb ((content ((agreement ((content ((person ((content 2) (pointer nil))) (number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'verb "eats" `((content ((verb ((content ((agreement ((content ((person ((content 3) (pointer nil))) (number ((content sg) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))
(add-to-grammar 'verb "eat" `((content ((verb ((content ((agreement ((content ((number ((content pl) (pointer nil))))) (pointer nil))))) (pointer nil))))) (pointer nil)))

(add-to-grammar 'vpastpp "finished" `((content ((vpastpp ,empty))) (pointer nil)))
(add-to-grammar 'vpastpp "offered" `((content ((vpastpp ,empty))) (pointer nil)))
(add-to-grammar 'vpastpp "built" `((content ((vpastpp ,empty))) (pointer nil)))

(add-to-grammar 'card "two" `((content ((card ,empty))) (pointer nil)))
(add-to-grammar 'card "three" `((content ((card ,empty))) (pointer nil)))
(add-to-grammar 'card "four" `((content ((card ,empty))) (pointer nil)))

(add-to-grammar 'ord "second" `((content ((ord ,empty))) (pointer nil)))
(add-to-grammar 'ord "third" `((content ((ord ,empty))) (pointer nil)))
(add-to-grammar 'ord "fourth" `((content ((ord ,empty))) (pointer nil)))

(add-to-grammar 'ppron "he" `((content ((ppron ,empty))) (pointer nil)))
(add-to-grammar 'ppron "you" `((content ((ppron ,empty))) (pointer nil)))

(add-to-grammar 'rel-pro "that" `((content ((rel-pro ,empty))) (pointer nil)))

(add-to-grammar 'adv "very" `((content ((very ,empty))) (pointer nil)))


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
  dag			; dag for unification
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
		       :dot-position 0
		       :dag `((content ((gamma ,empty) (np ,empty))) (pointer nil)))
	   (aref chart 0))
  (dotimes (i (+ (list-length words) 1))
    (do ((j 0 (+ j 1)))
      ((>= j (list-length (aref chart i))))
      (let* ((state (nth j (aref chart i)))
	     (next (next-cat state)))
	(cond ((and (incomplete? state) (not (find next *parts-of-speech*)))
	       ;; predictor
	       (dolist (rule (gethash next *grammar*))
		 (enqueue (make-state :production (append (list next) (car rule))
				      :subtree-position i
				      :dot-position 0
				      :dag (copy-dag (cadr rule)))
			  (aref chart i))))
	      ((and (incomplete? state) (find next *parts-of-speech*))
	       ;; scanner
	       (let* ((word (nth i words))
		      (dag (copy-dag (cadr (find word (gethash next *grammar*) :test #'(lambda (x y)
									       (string= (string-upcase x)
											(string-upcase (car y)))))))))
		 (if dag
		   (enqueue (make-state :production (list next word)
					:subtree-position i
					:dot-position 1
					:dag dag)
			    (aref chart (+ i 1))))))
	      (t
		;; completer
		(dolist (old-state (remove-if #'(lambda (x)
						  (not (eql (next-cat x)
							    (car (state-production state)))))
					      (aref chart (state-subtree-position state))))
		  (let ((new-dag (unify-states (state-dag state) (state-dag old-state) (car (state-production state)))))
		    (if new-dag
		      (enqueue (make-state :production (state-production old-state)
					   :subtree-position (state-subtree-position old-state)
					   :dot-position (+ (state-dot-position old-state) 1)
					   :dag new-dag
					   :children (append (state-children old-state) (list state)))
			       (aref chart i))))))))))
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



;; these are just for testing
(setf number-sg '((content ((number ((content sg) (pointer nil))))) (pointer nil)))
(setf person-3  '((content ((number ((person 3) (pointer nil))))) (pointer nil)))



;;; Function: fstruct-has-feauture?
;;; Arguments
;;;   fstruct - a feature structure as a list of arcs
;;;   name - name of an arc
;;; Returns:
;;;    True if an arc by the requested name exists in top depth of fstruct
(defun fstruct-has-feature-name? (fstruct name)
  (not (null (position name fstruct :test #'(lambda (x y)
					      (eq x (car y)))))))



;;; Macro: fstruct-name
;;; Arguments:
;;;   fstruct - a feature structure as a list of arcs
;;;   name - name of an arc
;;; 
;;; Provides access for setf to modify a feature structure.
;;; The requested name must exist in the feature structure
(defmacro fstruct-name (fstruct name)
  `(cadr (nth (position ,name ,fstruct :test #'(lambda (x y)
						 (eq x (car y)))) ,fstruct)))



;;; Macro: fstruct-name1
;;;
;;; This is just like fstruct-name except once the feature is found, it is 
;;; returned and not navigated further.
(defmacro fstruct-name1 (fstruct name)
  `(nth (position ,name ,fstruct :test #'(lambda (x y)
					   (eq x (car y)))) ,fstruct))



;;; Function: dereference1
;;; Arguments:
;;;   fstruct - a feature structure as a list of arcs
;;; Returns:
;;;   A feature structure after following all non-null pointers
;;;
;;; This is used by dereference (below) don't use it otherwise
(defun dereference1 (fstruct)
  (let ((pointer (fstruct-name fstruct 'pointer)))
    (if pointer
      (dereference1 pointer)
      fstruct)))


;;; Macro: derefence
;;; Arguments:
;;;   fstruct - a feature structure as a list of arcs
;;;
;;; Follows all pointers and the last content arcs of a feature structure
;;; to get its true value
(defmacro dereference (fstruct)
  `(fstruct-name (dereference1 ,fstruct) 'content))



;(defun dereference-all (fstruct)
;  (let ((dereferenced (dereference fstruct)))
;    (if (listp dereferenced)
;      (let (result)
;	(dolist (feature dereferenced)
;	  (setf result (append result (list (list (car feature) (dereference-all (cadr feature)))))))
;	result)
;      dereferenced)))



;;;
;;; The following functions are used to copy DAGS such that the resulting 
;;; DAG is a complete copy but its structure as far as pointers go is the
;;; same, so if there is a pointer in the first dag, there will be a 
;;; pointer to a new location in the new DAG.  This is a pretty nasty hack
;;; because the algorithms expect pointers to be available even to copies of
;;; DAGS.
;;;
(defparameter *old-objects* nil)
(defparameter *new-objects* nil)

(defun copy-dag (dag)
  (setf *old-objects* nil)
  (setf *new-objects* nil)
  (let ((new-dag (copy-tree dag)))
    (copy-dag1 dag new-dag)
    new-dag))

(defun copy-dag1 (dag new-dag)
  (dotimes (i (list-length dag))
    (let ((pos (position (nth i dag) *old-objects* :test #'eq)))
      (if pos
	(setf (nth i new-dag) (nth pos *new-objects*))
	(progn
	  (push (nth i dag) *old-objects*)
	  (push (nth i new-dag) *new-objects*)))
      (if (listp (nth i dag))
	(copy-dag1 (nth i dag) (nth i new-dag))))))




;;; Function: unify
;;; Arguments:
;;;
;;; Returns:
;;;
;;; This function implemens the algorithm shown in Figure 11.8 of
;;; "Speech and Language Processing"
(defun unify (f1 f2)
  (let ((f1-real (dereference f1))
	(f2-real (dereference f2)))
    (cond ((null f1-real)
	   (progn
	     (setf (fstruct-name f1 'pointer) f2)
	     f2))
	  ((null f2-real)
	   (progn
	     (setf (fstruct-name f2 'pointer) f1)
	     f1))
	  ((equal f1-real f2-real)
	   (progn
	     (setf (fstruct-name f1 'pointer) f2)
	     f2))
	  ((and (listp f1-real) (listp f2-real))
	   (progn
	     (setf (fstruct-name f2 'pointer) f1)
	     (dolist (feature f2-real)
	       (let (other-feature)
		 (if (fstruct-has-feature-name? f1-real (car feature))
		   (setf other-feature (fstruct-name1 f1-real (car feature)))
		   (progn
		     (setf other-feature (list (car feature) '((content nil) (pointer nil))))
		     (setf (dereference f1) (append f1-real (list other-feature)))))
		 (unless (unify (cadr feature) (cadr other-feature))
		   (return-from unify nil))))
	     f1)))))


(defun unify-states (dag1 dag2 cat)
  (let ((dag1-cp (copy-dag dag1))
	(dag2-cp (copy-dag dag2)))
    (if (unify (fstruct-name (dereference dag1-cp) cat) (fstruct-name (dereference dag2-cp) cat))
      dag2-cp)))
